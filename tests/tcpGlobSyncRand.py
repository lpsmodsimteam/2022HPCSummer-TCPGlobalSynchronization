import sst
import random

NUM_SENDERS = 100
SEED = 1234

# Node parameters are randomly generated between the two ranges for queue size and tick frequency.

START_CYCLE_MIN = 1
START_CYCLE_MAX = 100

random.seed(SEED)

senders = dict()

for x in range(NUM_SENDERS):
    senders[f"sender_{x}"] = sst.Component(f"Sender_{x}", "tcpGlobSync.sender")
    senders[f"sender_{x}"].addParams(
        {
            "tickFreq": "1s",
            "low_send_rate": "1",
            "max_send_rate": "50",
            "verbose_level": "1",
            "node_id": f"{x}",
            "starting_cycle": f"{random.randint(START_CYCLE_MIN, START_CYCLE_MAX)}",
            # "starting_cycle": "1",
        }
    )

receiver = sst.Component("Receiver", "tcpGlobSync.receiver")
receiver.addParams(
    {
        "tickFreq": "1s",  # frequency component updates at.
        "process_rate": "1000",  # number of packets consumed per tick.
        "queue_size": "4950",  # size of queue.
        "verbose_level": "1",  # level of console verbosity.
        "num_nodes": f"{NUM_SENDERS}",  # number of nodes that will connect to receiver.
        "window_size": "100",  # window size (in sim time) for global sync detection.
    }
)

# Connect the the sender nodes to the receiver
for x in range(NUM_SENDERS):
    sst.Link(f"Link{x}").connect(
        (receiver, f"port{x}", "1ms"), (senders[f"sender_{x}"], "port", "1ms")
    )
