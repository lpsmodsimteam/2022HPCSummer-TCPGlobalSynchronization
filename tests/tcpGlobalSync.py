import sst

router = sst.Component("Router", "tcpGlobalSync.router")
router.addParams(
    {
        "tickFreq": "1s",
        "numPorts": "1",
        "verbose_level": "3",
    }
)

client_zero = sst.Component("Client_0", "tcpGlobalSync.client")
client_zero.addParams(
    {
        "tickFreq": "1s",
        "timeout": "10",
        "node_id": "0",
        "verbose_level": "4",
    }
)


"""client_two = sst.Component("Client_2", "tcpGlobalSync.client")
client_two.addParams(
    {
        "tickFreq": "1s",
        "timeout": "10",
        "verbose_level": "3",
    }
)"""

sst.Link("Flow_One").connect(
    (client_zero, "commPort", "1s"), (router, "commPort0", "1s")
)
"""
sst.Link("Flow_Two").connect(
    (client_two, "commPort", "1ms"), (router, "commPort1", "1ms")
)"""
