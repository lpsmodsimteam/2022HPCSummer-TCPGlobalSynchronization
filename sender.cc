#include <sst/core/sst_config.h>
#include <sst/core/simulation.h>
#include "sender.h"

sender::sender( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {

    // Parameters
    clock = params.find<std::string>("tickFreq", "1s");
    low_send_rate = params.find<int64_t>("low_send_rate", 10);
    max_send_rate = params.find<int64_t>("max_send_rate", 100);
    verbose_level = params.find<int64_t>("verbose_level", 1);
    node_id = params.find<int64_t>("node_id", 0);
    starting_cycle = params.find<int64_t>("starting_cycle", 1); 

    // Variables
    curr_send_rate = low_send_rate - 1;
    counter = 0;
    drop_counter = 0;
    send_delay = 0;
 
    // Enabling SST Console Output
    output.init(getName() + "->", verbose_level, 0, SST::Output::STDOUT);

    // Create a string for each sender's filename to output data to.
    std::string fileName = "output/sender_data" + std::to_string(node_id) + ".csv";
    csvout.init("CSVOUT", 1, 0, SST::Output::FILE, fileName);
    csvout.output("Time,Send Rate\n");

    // Register Clock
    registerClock(clock, new SST::Clock::Handler<sender>(this, &sender::tick));

    // Configure Port
    port = configureLink("port", new SST::Event::Handler<sender>(this, &sender::eventHandler));
    if (!port) {
        output.fatal(CALL_INFO, -1, "Failed to configure port 'port'\n");
    } 
}

sender::~sender() {

}

bool sender::tick( SST::Cycle_t currentCycle ) { 
    if (currentCycle >= starting_cycle) {
        if (curr_send_rate < max_send_rate) {
            curr_send_rate = curr_send_rate + 1; // Increase transmission rate per tick.
        }
        
        output.verbose(CALL_INFO, 2, 0, "Sending %d\n", curr_send_rate);

        // Output send rate data to file. 
        csvout.output("%ld,%d\n", getCurrentSimTime(), curr_send_rate);

        // Send number of packets equal to the current send rate.
        packets_to_send = curr_send_rate; 
        for (int i = 0; i < packets_to_send; i++) {
            output.verbose(CALL_INFO, 3, 0, "Sending Packet #%d\n", i);
            sendPacket(i, send_delay); 
            send_delay += i; // Adding send delay equal to (1 + link travel speed).
                             // This is done to allow sender's to send packets consecutively with each other.
        }
    } 
    send_delay = 0; // Reset send delay for next cycle.
    return(false);
}

/**
 * @brief Receives messages that packets were loss and the client will limit its
 *        transmission rate in response. It will also send a message to notify
 *        the receiver that rates were limited so the receiver can collect data.
 *  
 */
void sender::eventHandler(SST::Event *ev) {
    PacketEvent *pe = dynamic_cast<PacketEvent*>(ev); // Cast the incoming event to a PacketEvent pointer.
    if ( pe != NULL ) {
        switch (pe->pack.type) {
            case PACKET:
                output.fatal(CALL_INFO, -1, "Sender should not receive packets. Error!\n");
                break;
            case LIMIT:
                output.verbose(CALL_INFO, 2, 0, "Packets were dropped. Limiting transmission rate\n");

                curr_send_rate = low_send_rate; // Limit transmission rate
                
                port->send(new PacketEvent(pe->pack));
                // Start delay?
                // Statistic when node dropped a packet.
                //^^^std::cout << node_id << ":" << getCurrentSimTimeMilli() << ":" << 1 << std::endl;
                //^^^std::cout << getCurrentSimTimeMilli() << std::endl;
                //rate_drop[drop_counter] = getCurrentSimTime();
                //drop_counter++;
        }
    }
    delete ev; // Delete event to avoid memory leaks.
}

// Construct and send packets out the sender's port.
void sender::sendPacket(int id, int delay) {
    PacketType type = PACKET;
    Packet packet = { type, id, node_id};

    // *tc is defined only in the header but it appears to be initialized by SST. 
    //  send() did not work as intended when overloaded with two parameters (i.e. send(delay, event))
    //  so tc was added and functionality worked as intended (adding 1ms of send delay).
    port->send(delay, tc, new PacketEvent(packet));
}