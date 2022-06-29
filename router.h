#ifndef _router_H
#define _router_H

#include <sst/core/component.h>
#include <sst/core/link.h>

class router : public SST::Component {

public:
    router( SST::ComponentId_t id, SST::Params& params );
    ~router();

    int verbose_level;
    bool tick ( SST::Cycle_t currentCycle ); 

    void commHandler(SST::Event *ev);

    SST_ELI_REGISTER_COMPONENT(
        router,
        "tcpGlobalSync",
        "router",
        SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
        "description",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
        {"tickFreq", "Descript", "1s"},
        {"numPorts", "Descript", "2"},   
        {"verbose_level", "Descript", "1"},
    )

    SST_ELI_DOCUMENT_PORTS(
        {"commPort", "A pointer to ports for connecting the router to flows.", {"StringEvent"}}
    )

private:
    SST::Output output;
    SST::Link *commPort; 
    int numPorts;
   
    std::string clock;
    
};

#endif