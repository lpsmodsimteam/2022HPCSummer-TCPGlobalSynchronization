#ifndef _sender_H
#define _sender_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include "packetevent.h"

/**
 * @brief Sender Component Class. Sends packets to a Receiver node. 
 * 
 */
class sender : public SST::Component {

public:
    /**
     * @brief Construct a new sender component for the simulation composition.
     * Occurs before the simulation starts.
     * 
     * @param id Component ID tracked by the simulator. 
     * @param params Parameters passed in via the Python driver file.
     */
    sender( SST::ComponentId_t id, SST::Params& params );

    /**
     * @brief Destroy the sender component. Occurs after the simulation is finished.
     * 
     */
    ~sender();
 
    /**
     * @brief Contains the sender's behavior and runs at its clock frequency.
     * 
     * @param currentCycle Current cycle of the component.
     * @return true Component is finished running.
     * @return false Component is not finished running.
     */
    bool tick ( SST::Cycle_t currentCycle );

    bool dummy ( SST::Cycle_t currentCycle );

    /**
     * @brief Handles packet information received from a receiver component.
     * 
     * @param ev PacketEvent that the component received.
     */
    void eventHandler(SST::Event *ev);


    /** 
     * \cond
     * Currently ignoring SST_ELI Macros as they break doxygen. 
     */
    SST_ELI_REGISTER_COMPONENT(
        sender,         // class
        "tcpGlobSync",   // element library
        "sender",       // component name
        SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),     // element version
        "Sends packets via a link to a receiver with a limited queue.",  // component description
        COMPONENT_CATEGORY_UNCATEGORIZED    // Component Category
    )

    // Parameter Name, Description, Default Value
    SST_ELI_DOCUMENT_PARAMS(
        {"tickFreq", "Frequency component updates at", "1s"},
        {"low_send_rate", "Lower limit of transmission rate", "10"},
        {"max_send_rate", "Max limit of transmission rate", "100"},
        {"verbose_level", "Verbosity level of console output", "1"},
        {"node_id", "ID of sender node connected to receiving node", "1"},
        {"starting_cycle", "Cycle when sender will begin transmitting packets", "1"}
    )

    // Port Name, Description, Event Type
    SST_ELI_DOCUMENT_PORTS(
        {"port", "Port that packets are sent across.", {"PacketEvent"}}
    )
    /** 
     * \endcond
     */

private:
    SST::Output output;     //!< SST Output object for printing to the console. 
    SST::Output csvout;     //!< SST Output object for printing to a csv file.

    SST::Link *port;        //!< Pointer to component's port. 
    SST::TimeConverter *delay_tc; //!< Required to convert sim time and component time.
    SST::TimeConverter *tc;

    /**
     * @brief Constructs and sends a packet out the sender's port.
     * 
     * @param id ID of the packet
     * @param delay send delay of the packet.
     */
    inline void sendPacket(int id, int delay);  

    int send_rate_data[300];
    int counter = 0;
    int drop_counter = 0;
    int rate_drop[1000];

    std::string clock;      //!< Frequency component will tick at. Takes in Unit Algebra string (i.e. "1ms").
    int low_send_rate;      //!< Starting rate and what sender will lower transmission rate to during packet loss. 
    int max_send_rate;      //!< Maximum transmission rate of a node. 
    int curr_send_rate;     //!< Number of packets that are being sent in the current cycle. 
    int packets_to_send;    //!< Number of packets that are being sent in the current cycle.
    int verbose_level;      //!< Console output verbosity level. 
    int node_id;            //!< User defined component ID in simulation. 
    int starting_cycle;     //!< The cycle the node begins transmitting packets at. 

    int stat_window_size;   //!< Window size for measuring transmission resets.
    int send_delay;         //!< Packet sending delay. 
};


#endif