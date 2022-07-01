import sst

NUM_NODES = 15

nodes = dict()

router = sst.Component("Router", "tcpGlobalSync.router")
router.addParams(
    {
        "tickFreq": "1s",
        "numPorts": f"{NUM_NODES}",
        "verbose_level": "1",
    }
)

for x in range(NUM_NODES):
    nodes[f"node_{x}"] = sst.Component(f"Node {x}", "tcpGlobalSync.client")
    nodes[f"node_{x}"].addParams(
        {
            "tickFreq": "0.1s",
            "timeout": "100",
            "node_id": f"{x}",
            "verbose_level": "1",
        }
    )

for x in range(NUM_NODES):
    sst.Link(f"Link_{x}").connect(
        (nodes[f"node_{x}"], "commPort", "1ms"), (router, f"commPort{x}", "1ms")
    )
