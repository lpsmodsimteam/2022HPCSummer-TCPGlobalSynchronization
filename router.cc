#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include "router.h"

using SST::Interfaces::StringEvent;

router::router( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {
    // Initialize Parameters
    clock = params.find<std::string>("tickFreq", "1s");
    verbose_level = params.find<int64_t>("verbose_level", 1);
    numPorts = params.find<int64_t>("numPorts", 2);

    output.init(getName() + "->", verbose_level, 0, SST::Output::STDOUT); 

    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

    registerClock(clock, new SST::Clock::Handler<router>(this, &router::tick));

    /**commPort = configureLink("commPort", new SST::Event::Handler<router, int>(this, &router::commHandler, 1));
    if ( !commPort ) {
        output.fatal(CALL_INFO, -1, "commPort failed");
    } */
 
    commPort = new SST::Link*[numPorts];

    for (int i = 0; i < numPorts; ++i) {
        std::string port = "commPort" + std::to_string(i);
        commPort[i] = configureLink(port, new SST::Event::Handler<router, int>(this, &router::commHandler, i));

        if ( !commPort[i] ) {
            output.fatal(CALL_INFO, -1, "Commport%d failed.", i);
        }
    }  
}

router::~router() {

}

bool router::tick( SST::Cycle_t currentCycle ) {
    
    if (currentCycle == 100) {
        primaryComponentOKToEndSim();
        return(true);
    }
    output.verbose(CALL_INFO, 1, 0, "Testing...\n"); 

    return(false);
}

void router::commHandler(SST::Event *ev, int port) {
    StringEvent *se = dynamic_cast<StringEvent*>(ev);
    if (se != NULL) {
        std::cout << se->getString() << std::endl;
    }
    delete ev;
}