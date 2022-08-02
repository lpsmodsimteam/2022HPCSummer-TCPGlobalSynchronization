#ifndef _receiver_H
#define _receiver_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <queue>
#include "packetevent.h"

/**
 * @brief Receiver Component Class. Receives packets from a sender and processes them. 
 * 
 */
class receiver : public SST::Component {
public:

    /**
     * @brief Construct a new receiver component for the simulation composition.
     * Occurs before the simulation starts.
     * 
     * @param id Component ID tracked by the simulator.
     * @param params Parameters passed in via the Python driver file.
     */
    receiver( SST::ComponentId_t id, SST::Params& params );

    /**
     * @brief Deconstruct the receiver component. Occurs after the simulation is finished.
     * 
     */
    ~receiver();

    /**
     * @brief Contains the receiver's behavior and runs at its clock frequency.
     * 
     * @param currentCycle Current cycle of the component.
     * @return true Component is finished running.
     * @return false Component is not finished running.
     */
    bool tick ( SST::Cycle_t currentCycle ); 

    /**
     * @brief Handles packet information received from a sender component.
     * 
     * @param ev PacketEvent that the component received.
     */
    void eventHandler(SST::Event *ev);

    /**
     * @brief Runs after all components have been constructed but before the simulation begins.
     * 
     */
    void setup();

    /**
     * @brief Runs after the component is finished but before all components are deconstructed.
     * 
     */
    void finish();

    /**
     * \cond 
     * Currently ignoring SST_ELI Macros as they break doxygen
     */
    SST_ELI_REGISTER_COMPONENT(
        receiver,       // Component Name
        "tcpGlobSync",   // Element Name
        "receiver",     // Component Name
        SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),     // Element Version
        "Receives and processes packets from senders.",     // Component Description
        COMPONENT_CATEGORY_UNCATEGORIZED    // Component Category
    ) 

    // Parameter Name, Description, Default Value
    SST_ELI_DOCUMENT_PARAMS(
        {"tickFreq", "Frequency component updates at", "1s"},
        {"process_rate", "Number of packets that are process per cycle", "10"},
        {"queue_size", "Size of queue", "50"},
        {"verbose_level", "Verbosity level for console output", "1"},
        {"num_nodes", "Number of nodes connected to receiver", "1"},
        {"window_size", "Amount of ticks in which the number of sender nodes that transmission rates are reset occur.", "1"},
        {"run_time", "Number of cycles the simulation will run.", "300"},
        {"enable_pred", "Enable psuedo-red dropping policy to prevent global synchronization.", "0"},
    )

    // Port Name, Description, Event Type
    SST_ELI_DOCUMENT_PORTS(
        {"port%d", "Port that packets are sent across.", {"PacketEvent"}}
    ) 
    /**
     * \endcond 
     */

private:
    SST::Output output; //!< SST Output object for printing to the console.
    SST::Output csvout; //!< SST Output object for printing to a csv file.
    SST::Link **port;   //!< Pointer to an array of port pointers. Allows for variable number of ports to be dynamically allocated.

    std::string clock;  //!< Frequency component will tick at. Takes in Unit Algebra string. (i.e. "1ms").
    std::queue<Packet> msgQueue;    //!< Queue for packets.
    int queue_size;     //!< Size of packet queue.
    int process_rate;   //!< Amount of packets that can be processed per tick.
    int verbose_level;  //!< Verbosity level of console output.
    int num_nodes;      //!< Number of connected senders.
    int num_resets;     //
    int64_t run_time;       //!< Number of cycles the simulation will run.

    float link_utilization;     //!< Aggregate link utilization of the receiver.
    float packets_processed;    //!< Packets processed per tick.
    int packet_loss;    //!< Number of packets loss. 

    int window_size;            //!< Window size for synchronization detection. 
    int sampling_start_time;    //!< Simulator Time in which sampling occurs. 
    bool sampling_status;       //!< Is sampling occuring or not.
    bool already_sampled;       //!< Determines if the behavior has already been detected before the window size is reached.
    //int tracked_nodes[NUM_NODES]; //!!!! DYNAMICALLY ALLOCATE>
    int *tracked_nodes;
    int nodes_limited;          //!< Number of nodes that have limited their transmission rate in a window.
    float globsync_detect;      //!< Metric if the global synchronization behavior has occured.

    int num_globsync;
    float prev_globsync_time;
    float new_globsync_time;
    float globsync_time_diff_avg;
    int total_time_diff;
    float metric_variance;
    float metric_middle;
    
    bool enable_pred;   //!< Enable psuedo-red algorithm to prevent global synchronization from occurring.
    float queue_avg;    //!< Average queue depth. 
    float prev_avg;     //!< Previous average queue depth.
    float min_pred;     //!< Minimum threshold for pseudo-red algorithm.
    float max_pred;
    int count_pred;
    int64_t rand_seed;
    SST::RNG::MarsagliaRNG *rng;
    float rand_num;
};

#endif