# Reference: http://sst-simulator.org/SSTPages/SSTUserPythonFileFormat/

import sst  # Use SST Library
import random

NUM_SENDERS = 100  # Number of sender nodes to instantiate.
SEED = 1234  # Seed for RNG

# Lower and upper range for the sender component's starting cycle.
START_CYCLE_MIN = 1
START_CYCLE_MAX = 100

random.seed(SEED)

senders = dict()
for x in range(NUM_SENDERS):

    # Creating a sender component from element tcpGlobSync (tcpGlobSync.sender) named "Sender_{node_id}".
    senders[f"sender_{x}"] = sst.Component(f"Sender_{x}", "tcpGlobSync.sender")

    # Adding parameters to sender node.
    senders[f"sender_{x}"].addParams(
        {
            "tickFreq": "1s",  # frequency component updates at.
            "low_send_rate": "1",  # Minimum number of packets sent per second.
            "max_send_rate": "50",  # Maximum number of packets set per second.
            "verbose_level": "1",  # Verbosity level of console output.
            "node_id": f"{x}",  # ID of sender node.
            "starting_cycle": f"{random.randint(START_CYCLE_MIN, START_CYCLE_MAX)}",  # Randomized cycle in which the component begins to send packets.
        }
    )

# Create a receiver and add parameters to it.
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

# Connect each sender node to the receiver
for x in range(NUM_SENDERS):
    sst.Link(f"Link{x}").connect(
        (receiver, f"port{x}", "1ms"), (senders[f"sender_{x}"], "port", "1ms")
    )
