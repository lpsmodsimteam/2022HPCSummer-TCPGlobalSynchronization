#include <sst/core/sst_config.h>
#include "client.h"
#include <sst/core/interfaces/stringEvent.h>

using SST::Interfaces::StringEvent;

client::client( SST::ComponentId_t id, SST::Params& params ) : SST::Component(id) {
    // Initialize Parameters
    clock = params.find<std::string>("tickFreq", "1s");
    verbose_level = params.find<int64_t>("verbose_level");

    output.init(getName() + "->", verbose_level, 0, SST::Output::STDOUT ); 

    registerClock(clock, new SST::Clock::Handler<client>(this, &client::tick));
    commPort = configureLink("commPort", new SST::Event::Handler<client>(this, &client::commHandler));

    if (!commPort) {
        output.fatal(CALL_INFO, -1, "CommPort failed.");
    }
}

client::~client() {

}

bool client::tick( SST::Cycle_t currentCycle ) {
    commPort->send(new StringEvent(getName()));

    return(false);
}