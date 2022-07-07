import sst

NUM_NODES = 5

nodes = dict()

router = sst.Component("Router", "tcpGlobalSync.router")
router.addParams(
    {
        "tickFreq": "1s",
        "numPorts": f"{NUM_NODES}",
        "verbose_level": "2",
        "queueSize": "50",
    }
)

for x in range(NUM_NODES):
    nodes[f"node_{x}"] = sst.Component(f"Node {x}", "tcpGlobalSync.client")
    nodes[f"node_{x}"].addParams(
        {
            "tickFreq": "0.5s",
            "timeout": "100",
            "node_id": f"{x}",
            "verbose_level": "2",
            "window_size": "5",
        }
    )

for x in range(NUM_NODES):
    sst.Link(f"Link_{x}").connect(
        (nodes[f"node_{x}"], "commPort", "1ms"), (router, f"commPort{x}", "1ms")
    )
