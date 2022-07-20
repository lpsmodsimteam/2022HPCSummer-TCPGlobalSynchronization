#include <sst/core/sst_config.h>
#include <sst/core/simulation.h>
#include "receiver.h"
#include <math.h>

/**
 * @brief Constructs a new receiver component for the SST composition.
 * 
 * @param id Component's id for the SST simulator.
 * @param params Used to grab parameters from the python driver file.
 */
receiver::receiver( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {

    // Parameters
    clock = params.find<std::string>("tickFreq", "1s");
    process_rate = params.find<int64_t>("process_rate", 10);
    queue_size = params.find<int64_t>("queue_size", 50);
    verbose_level = params.find<int64_t>("verbose_level", 1);
    num_nodes = params.find<int64_t>("num_nodes", 1);
    window_size = params.find<int64_t>("window_size", 100);

    // Enabling SST Console Output
    output.init(getName() + "->", verbose_level, 0, SST::Output::STDOUT);

    output.output("%d", queue_size);

    // Enabling SST File Output
    csvout.init("CSVOUT", 1, 0, SST::Output::FILE, "receiver_data.csv");
    csvout.output("Time,Queue Size,Packet Loss,Link Utilization,Global Sync Detected,Average Queue Depth\n");

    // Register the node as a primary component.
	// Then declare that the simulation cannot end until this
	// primary component declares primaryComponentOKToEndSim();
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

    // Variables
    sampling_start_time = 0;
    sampling_status = false;
    already_sampled = false;
    nodes_limited = 0;
    globsync_detect = 0;
    
    //WRED Stuff
    queue_avg = 0;
    prev_avg = 0;
    enable_wred = 1;
    
    // Stats 
    packet_loss = 0;
    packets_processed = 0;
    link_utilization = 0;

    // Pointer to an array of port pointers.
    port = new SST::Link*[num_nodes];

    // Configure all ports to different link.
    for (int i = 0; i < num_nodes; ++i) {
        std::string strport = "port" + std::to_string(i);
        port[i] = configureLink(strport, new SST::Event::Handler<receiver>(this, &receiver::eventHandler));
        if (!port) {
            output.fatal(CALL_INFO, -1, "Failed to configure port 'port'\n");
        } 
    } 

    // Clock
    registerClock(clock, new SST::Clock::Handler<receiver>(this, &receiver::tick));   
}

receiver::~receiver() {

}

void receiver::setup() {
    /**tracked_nodes[0] = 0;
    tracked_nodes[1] = 0;
    tracked_nodes[2] = 0;*/
    for (int i = 0; i < num_nodes; i++) {
        tracked_nodes[i] = 0;
        std::cout << tracked_nodes[i] << std::endl;
    }
}

/**
 * @brief 
 * 
 * @param currentCycle 
 * @return true 
 * @return false 
 */
bool receiver::tick( SST::Cycle_t currentCycle ) {
    //output.verbose(CALL_INFO, 2, 0, "SimTime: %ld\n", getCurrentSimTime());
    //output.verbose(CALL_INFO, 2, 0, "Queue Size: %ld\n", msgQueue.size());
    //output.verbose(CALL_INFO, 2, 0, "Global Synchronization Percentage: %f\n\n", globsync_detect);

    // Statistic Info 
    //^^^std::cout << msgQueue.size() << std::endl;
    //^^^std::cout << packet_loss << std::endl;
    //^^^std::cout << link_utilization * 100 << std::endl; 
    //^^^std::cout << globsync_detect << std::endl;

    
    if (sampling_status == true && (getCurrentSimTimeMilli() >= sampling_start_time + window_size)) {
        output.verbose(CALL_INFO, 2, 0, "Ending Sampling\n");
        already_sampled = false;
        sampling_status = false; 
        sampling_start_time = 0;
        nodes_limited = 0;
        for (int i = 0; i < num_nodes; i++) {
            tracked_nodes[i] = 0;
        }
    }

    // Check if message queue is empty
    if (!msgQueue.empty()) {
        // Process messages in queue.
        for (int i = 0; i < process_rate; i++) {
            if (msgQueue.empty()) {
                break; // If the queue becomes empty during processing, break out.
            }
        packets_processed++; //  
        msgQueue.pop(); // "Process" the packet and remove it from queue.
        }
    }

    link_utilization = packets_processed / (float) process_rate;
    packets_processed = 0;


    // Data output and File output
    output.verbose(CALL_INFO, 1, 0, "SimTime: %ld\nQueue Size: %d\nPacket Loss: %d\nLink Utilization: %f\nGlobal Sync Behavior Detected: %f\n\n", 
        getCurrentSimTime(), msgQueue.size(), packet_loss, (link_utilization*100), globsync_detect);
    csvout.output("%ld,%ld,%d,%f,%f,%f\n", getCurrentSimTime(), msgQueue.size(), packet_loss, (link_utilization * 100), globsync_detect, queue_avg);
    output.output(CALL_INFO, "Queue Average: %f, Prev Queue Average: %f\n", queue_avg, prev_avg);

    if (globsync_detect) {
        globsync_detect = 0;
    }

    if (currentCycle == 300) {
        primaryComponentOKToEndSim();
        return(true);
    }

    return(false);
}

void receiver::eventHandler(SST::Event *ev) {
    PacketEvent *pe = dynamic_cast<PacketEvent*>(ev); // Cast the incoming event to a PacketEvent pointer.
    if (pe != NULL) {
        switch (pe->pack.type) {
            case PACKET: 
            
                if (enable_wred) {
                    queue_avg = prev_avg + (msgQueue.size() - prev_avg) / pow(2, 6);
                    prev_avg = queue_avg;
                    //output.output("Average Queue Depth: %f\n", queue_avg);
                }

                // Check if queue is full
                if (msgQueue.size() + 1 > queue_size) {
                    // If so, drop the packet.
                    output.verbose(CALL_INFO, 3, 0, "Packet Loss from %d\n", pe->pack.node_id);

                    // Send limit message alerting sender that packet was dropped.
                    Packet limitMsg = { LIMIT, pe->pack.id, pe->pack.node_id };
                    port[limitMsg.node_id]->send(new PacketEvent(limitMsg));
                    packet_loss++; 
                } else {
                    // Else, add message to queue
                    msgQueue.push(pe->pack);
                }
                break;
            case LIMIT:
                // Receives message that rates are limited, 
                // Begins sampling for other transmission rate limiting in the user defined window time.
                if (sampling_status == false && already_sampled == false) {
                    output.verbose(CALL_INFO, 2, 0, "Started Sampling\n");
                    sampling_start_time = getCurrentSimTimeMilli(); // Begin Window of Time
                    sampling_status = true; // Start sampling.
                    tracked_nodes[pe->pack.node_id] = 1;
                    nodes_limited++;
                } 

                if (sampling_status == true && tracked_nodes[pe->pack.node_id] == 0 && already_sampled == false) {
                    output.verbose(CALL_INFO, 2, 0, "Sampling: Packet Loss from %d\n", pe->pack.node_id);
                    tracked_nodes[pe->pack.node_id] = 1;
                    nodes_limited++; 
                    if (nodes_limited == num_nodes) {
                        output.verbose(CALL_INFO, 2, 0, "Ending Sampling Early\n");
                        globsync_detect = 1; 
                        already_sampled = true; 
                        nodes_limited = 0;
                        for (int i = 0; i < num_nodes; i++) {
                            tracked_nodes[i] = 0;
                        }
                    }
                } 
                break;
        }
    }
    delete ev;  // Delete event to prevent memory leaks.
}