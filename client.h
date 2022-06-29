#ifndef _client_H
#define _client_H

#include <sst/core/component.h>
#include <sst/core/link.h>

class client : public SST::Component {

public:
    client ( SST::ComponentId_t id, SST::Params& params );
    ~client();

    bool tick ( SST::Cycle_t currentCycle );

    void commHandler(SST::Event *ev);
    int verbose_level;

    SST_ELI_REGISTER_COMPONENT(
        client,
        "tcpGlobalSync",
        "client",
        SST_ELI_ELEMENT_VERSION( 1, 0, 0 ),
        "Descript",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
        {"tickFreq", "Descript", "1s"},
        {"timeout", "Descript", "100"}, 
        {"verbose_level", "Descript", "1"},
    )

    SST_ELI_DOCUMENT_PORTS(
        {"commPort", "Port that packets and acknowledgements are sent across.", {"StringEvent"}}
    )

private:
    SST::Output output;
    SST::Link *commPort;  

    std::string clock;
    int time_out; 
    
};

#endif