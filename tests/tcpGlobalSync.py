import sst

router = sst.Component("Router", "tcpGlobalSync.router")
router.addParams(
    {
        "tickFreq": "1s",
        "numPorts": "2",
        "verbose_level": "1",
    }
)

client_one = sst.Component("Client_1", "tcpGlobalSync.client")
client_one.addParams(
    {
        "tickFreq": "1s",
        "timeout": "10",
        "verbose_level": "1",
    }
)


client_two = sst.Component("Client_2", "tcpGlobalSync.client")
client_two.addParams(
    {
        "tickFreq": "1s",
        "timeout": "10",
        "verbose_level": "1",
    }
)

sst.Link("Flow_One").connect(
    (client_one, "commPort", "1ms"), (router, "commPort0", "1ms")
)
sst.Link("Flow_Two").connect(
    (client_two, "commPort", "1ms"), (router, "commPort1", "1ms")
)
