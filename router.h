#ifndef _router_H
#define _router_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include "messageevent.h"
#include <queue>

class router : public SST::Component {

public:
    router( SST::ComponentId_t id, SST::Params& params );
    ~router();

    int verbose_level;
    bool tick ( SST::Cycle_t currentCycle ); 

    void setup();

    void commHandler(SST::Event *ev, int port);

    SST_ELI_REGISTER_COMPONENT(
        router,
        "tcpGlobalSync",
        "router",
        SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
        "Router receives messages from clients and stores them into a queue to be processed.",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
        {"tickFreq", "Frequency at which one frame in the queue is processed.", "1s"},
        {"numPorts", "Number of ports the router has (Matches number of clients in simulation)", "1"},   
        {"verbose_level", "Verbosity level of console output.", "1"},
        {"queueSize", "Size of the router's input queue", "50"},
    )

    SST_ELI_DOCUMENT_PORTS(
        {"commPort%d", "A pointer to ports for connecting the router to flows.", {"MessageEvent"}}
    )

private:
    SST::Output output;
    SST::Link **commPort;   // A pointer to a set of ports for the router. 
    int numPorts;   // Number of ports the router has.

    std::string clock; // Frequency the router updates at.

    std::queue<Message> msgQueue; // Queue for incoming frames 
    int queueSize;  // Size of Message queue

    float goodput;  // Amount of new frames that travel the link to the router.
    float throughput;   // Total amount of frames that travel the link to the router.
};

#endif