import sst

router = sst.Component("Router", "tcpGlobalSync.router")
router.addParams(
    {"tickFreq": "1s", "numPorts": "2", "verbose_level": "3", "queueSize": "50"}
)

client_zero = sst.Component("Client_0", "tcpGlobalSync.client")
client_zero.addParams(
    {
        "tickFreq": "0.5s",
        "timeout": "20",
        "node_id": "0",
        "verbose_level": "4",
        "window_size": "5",
    }
)


client_one = sst.Component("Client_1", "tcpGlobalSync.client")
client_one.addParams(
    {
        "tickFreq": "0.5s",
        "timeout": "20",
        "node_id": "1",
        "verbose_level": "4",
        "window_size": "5",
    }
)

sst.Link("Flow_One").connect(
    (client_zero, "commPort", "1ms"), (router, "commPort0", "1ms")
)

sst.Link("Flow_Two").connect(
    (client_one, "commPort", "1ms"), (router, "commPort1", "1ms")
)
