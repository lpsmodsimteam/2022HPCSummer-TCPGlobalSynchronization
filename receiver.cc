#include <sst/core/sst_config.h>
#include <sst/core/simulation.h>
#include "receiver.h"
#include <math.h>

receiver::receiver( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {

    // Parameters
    clock = params.find<std::string>("tickFreq", "1s");
    process_rate = params.find<int64_t>("process_rate", 10);
    queue_size = params.find<int64_t>("queue_size", 50);
    verbose_level = params.find<int64_t>("verbose_level", 1);
    num_nodes = params.find<int64_t>("num_nodes", 1);
    window_size = params.find<int64_t>("window_size", 100);
    enable_pred = params.find<int64_t>("enable_pred", 0);
    run_time = params.find<int64_t>("run_time", 300);

    // Enabling SST Console Output
    output.init(getName() + "->", verbose_level, 0, SST::Output::STDOUT);

    // Enabling SST File Output
    csvout.init("CSVOUT", 1, 0, SST::Output::FILE, "output/receiver_data.csv");
    csvout.output("Time,Queue Size,Packet Loss,Link Utilization,Global Sync Detected,Average Queue Depth,Times Problem Detected,Average Time Difference between Problem,Variance of Time Difference\n");

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
    
    // Variables for custom dropping policy.
    queue_avg = 0;
    prev_avg = 0;
    rand_num = 0;
    min_pred = queue_size * 0.9; 
    count_pred = 0;
   
	rng = new SST::RNG::MarsagliaRNG(10, 124); // Create a Marsaglia RNG with a default value and a random seed.
    
    // Stats 
    packet_loss = 0;
    packets_processed = 0;
    link_utilization = 0;

    // Variables for global synchronization detection
    num_globsync = 0;
    prev_globsync_time = 0;
    new_globsync_time = 0;
    globsync_time_diff_avg = 0;
    total_time_diff = 0;
    metric_middle = 0;
    metric_variance = 0;

    // Pointer to an array of port pointers.
    port = new SST::Link*[num_nodes];

    // Configure all ports to a different link.
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

/**
 * @brief Dynamically allocate an array to keep track of what sender components have limited transmission rates during a window of time.
 * 
 */
void receiver::setup() {  
    tracked_nodes = (int*) calloc(num_nodes, sizeof(int));
}

/**
 * @brief Free up array allocated in setup. 
 * 
 */
void receiver::finish() {
    free(tracked_nodes);
}
 
bool receiver::tick( SST::Cycle_t currentCycle ) {
   
    // Data output and File output
    output.verbose(CALL_INFO, 1, 0, "SimTime: %ld\nQueue Size: %ld\nPacket Loss: %d\nLink Utilization: %f\nGlobal Sync Behavior Detected: %f\nCurrent Sim Time: %ld\n", 
        getCurrentSimTime(), msgQueue.size(), packet_loss, (link_utilization*100), globsync_detect, getCurrentSimTimeMilli());

    output.verbose(CALL_INFO, 1, 0, "Number of times behavior has occured: %d\nDifference in time between last two detection: %f\nAverage difference in time between detections: %f\n\n", num_globsync, new_globsync_time - prev_globsync_time, globsync_time_diff_avg);

    csvout.output("%ld,%ld,%d,%f,%f,%f,%d,%f,%f\n", getCurrentSimTime(), msgQueue.size(), packet_loss, (link_utilization * 100), globsync_detect, queue_avg, num_globsync, globsync_time_diff_avg, metric_variance);

    // <<<COMMENT>>>
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
        packets_processed++;  
        msgQueue.pop(); // "Process" the packet and remove it from queue.
        }
    }

    link_utilization = packets_processed / (float) process_rate;
    packets_processed = 0;

    if (globsync_detect) {
        globsync_detect = 0;
    }

    // End simulation at user-defined time.
    if (currentCycle == run_time) {
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
                //count_pred++; 
                output.verbose(CALL_INFO, 3, 0, "Received msg from %d\n", pe->pack.node_id);
                // CUSTOM QUEUE DROPPING POLICY.
                if (enable_pred) {
                    count_pred++; 

                    if (msgQueue.size() <= min_pred) {
                        msgQueue.push(pe->pack);
                    }

                    if (msgQueue.size() > min_pred) {
                        if (count_pred > 250) {
                            //try for drop
                            rand_num = (float) (rng->nextUniform());
                            if (rand_num < 0.30) {
                                output.output(CALL_INFO, "DROPPED EARLY----------------------\n");
                                Packet limitMsg = { LIMIT, pe->pack.id, pe->pack.node_id };
                                port[limitMsg.node_id]->send(new PacketEvent(limitMsg));
                                packet_loss++;
                                count_pred = 0;
                            }
                        } else {
                            //just enqueue
                            msgQueue.push(pe->pack);
                        }
                    }

                    if (msgQueue.size() + 1 > queue_size) {
                        Packet limitMsg = { LIMIT, pe->pack.id, pe->pack.node_id };
                        port[limitMsg.node_id]->send(new PacketEvent(limitMsg));
                        packet_loss++;
                        count_pred = 0;
                    }
                } 
                // TAIL DROP QUEUE DROPPING POLICY
                else {
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
                }
                break;
            case LIMIT:
                // Receives message that rates are limited, 
                // Begins sampling for other transmission rate limiting in the user defined window time.
                if (sampling_status == false && already_sampled == false) {
                    output.verbose(CALL_INFO, 2, 0, "Started Sampling. Packet Loss from %d\n", pe->pack.node_id);

                    sampling_start_time = getCurrentSimTimeMilli(); // Begin Window of Time
                    sampling_status = true; // Start sampling.
                    tracked_nodes[pe->pack.node_id] = 1; 
                    nodes_limited++;
                } 

                // If currently sampling, and the node that has limited its transmission rate is new, enter loop.
                if (sampling_status == true && tracked_nodes[pe->pack.node_id] == 0 && already_sampled == false) {
                    output.verbose(CALL_INFO, 2, 0, "Currently Sampling: Packet Loss from %d\n", pe->pack.node_id);

                    tracked_nodes[pe->pack.node_id] = 1;
                    nodes_limited++; 

                    if (nodes_limited == num_nodes) {
                        output.verbose(CALL_INFO, 2, 0, "Ending Sampling Early\n");

                        new_globsync_time = getCurrentSimTimeMilli();
                        num_globsync++;
                        if (num_globsync != 1) { 
                            globsync_time_diff_avg = (total_time_diff + (new_globsync_time - prev_globsync_time)) / (num_globsync - 1); 
                            total_time_diff = total_time_diff + (new_globsync_time - prev_globsync_time);
                            metric_middle = (new_globsync_time - prev_globsync_time) - globsync_time_diff_avg;
                            metric_variance = (metric_middle * metric_middle) / num_globsync - 1;
                        } 
                        prev_globsync_time = new_globsync_time;
                        globsync_detect = 1; 
                        already_sampled = true; 
                        nodes_limited = 0;

                        // Reset tracked nodes to zero.
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