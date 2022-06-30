#ifndef _client_H
#define _client_H

#include <sst/core/component.h>
#include <sst/core/link.h>
#include "messageevent.h"

#define IDLE 0
#define WAITING 1
#define SENDING 2   

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
        {"node_id", "Descript", "1"},
    )

    SST_ELI_DOCUMENT_PORTS(
        {"commPort", "Port that frames and acknowledgements are sent across.", {"StringEvent"}}
    )

private:
    SST::Output output;
    SST::Link *commPort;  

    std::string clock;
    int timeout; 

    void checkForSend(SST::Cycle_t currentCycle);
    int frames_to_send;
    int current_frame;
    int acks_received;
    int start_send_cycle;
    int window_size;
    int timer_start;
    void sendMessage(MessageType type, StatusType status, int node, int frame);

    int client_state;
    int node_id;
     

    int test;
};

#endif