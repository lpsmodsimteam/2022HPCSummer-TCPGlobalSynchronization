#include <sst/core/sst_config.h>
#include "client.h"

client::client( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {
    // Initialize Parameters
    clock = params.find<std::string>("tickFreq", "1ms");
    verbose_level = params.find<int64_t>("verbose_level", 1);
    timeout = params.find<int64_t>("timeout", 1);
    node_id = params.find<int64_t>("node_id", 1);

    window_size = 4;
   

    output.init(getName() + "->", verbose_level, 0, SST::Output::STDOUT ); 

    // Initialize Variables;
    client_state = IDLE;
    frames_to_send = 0;
    start_send_cycle = 0;
    current_frame = 0;
    acks_received = 0;

    registerClock(clock, new SST::Clock::Handler<client>(this, &client::tick));
    commPort = configureLink("commPort", new SST::Event::Handler<client>(this, &client::commHandler));

    if (!commPort) {
        output.fatal(CALL_INFO, -1, "CommPort failed.\n");
    }
}

client::~client() {

}
int test = 1;
bool client::tick( SST::Cycle_t currentCycle ) {
    output.verbose(CALL_INFO, 1, 0, "%ld\n", getCurrentSimTimeMilli());
    std::cout << current_frame << std::endl;
    if (client_state == IDLE) {
        output.verbose(CALL_INFO, 3, 0, "Idle State\n");
        if (test) {
            checkForSend(currentCycle);
            test = 0;
        }
        
        // RNG to send
        // Reset current frame to 0
    } else if (client_state == WAITING) {
        output.verbose(CALL_INFO, 3, 0, "Waiting State\n");
        // Do nothing? Collect statistics? Decrement time out (and in sending as well)
    } else if (client_state == SENDING) {
        output.verbose(CALL_INFO, 3, 0, "Sending State\n");
        // Sends out initial window size # of messages without waiting for acks. (Sliding Window)
        if (start_send_cycle + window_size > currentCycle) {
            output.verbose(CALL_INFO, 4, 0, "Sending Initial Frames\n");
            // Send Message
            sendMessage(FRAME, NEW, node_id, current_frame);
            current_frame++;
            // keep state until all initialization messages sent out.
            // client_state = SENDING;
            if (start_send_cycle + window_size == currentCycle) {
                client_state = WAITING;
            }
        } else {
            output.verbose(CALL_INFO, 3, 0, "Responding to Ack with next frame\n");
            // Send message
            sendMessage(FRAME, NEW, node_id, current_frame);
            current_frame++;
            // Waiting
            client_state = WAITING;
        } 
    } else {
        output.fatal(CALL_INFO, -1, "Client is in a unknown state.\n");
    } 

    return(false);
}

// Receiving acks
void client::commHandler(SST::Event *ev) {
    MessageEvent *me = dynamic_cast<MessageEvent*>(ev);
    if ( me != NULL ) {
        switch (me->msg.type) {
            case FRAME:
                output.fatal(CALL_INFO, -1, "Client should not be receiving frames. Error!\n");
                break; 
            case ACK:
                acks_received++; // <------------- CHEKC THIS LOGIC WITH (A). MIGHT BE OFF
                output.verbose(CALL_INFO, 5, 0, "Received ack for frame: %d\n", me->msg.id);
                // In case an ack is received while the client is still sending out initial window size of frames.
                if (current_frame < window_size) {
                    output.verbose(CALL_INFO, 4, 0, "Received acks during initialization\n");
                } else {
                    output.verbose(CALL_INFO, 4, 0, "Received acks outside of initialization\n");
                    client_state = SENDING;
                    // Received acks for all frames that were sent out.
                    if (acks_received == frames_to_send) { // (A)
                        output.verbose(CALL_INFO, 4, 0, "Packet Succesfully Sent\n");
                        client_state = IDLE;
                    }
                }
                
                break;
        }
    }
    // If receive ack but in initial sending state -> SENDING state
    // Receive ack / Send next message out -> Sending state
    // If ack id received = frames_to_send -> IDLE state
}


void client::checkForSend(SST::Cycle_t currentCycle) {
    // Rng to see if packets need to be sent

    // Rng to check how many frame need to be sent
    frames_to_send = 10;

    // Set send_cycle = next cycle for sending initial W frames.
    start_send_cycle = currentCycle + 1;

    // Change state to sending
    client_state = SENDING;
}

void client::sendMessage(MessageType type, StatusType status, int node_id, int frame) {
    Message msg = { type, status, node_id, frame };
    commPort->send(new MessageEvent(msg));
}