# Reference: http://sst-simulator.org/SSTPages/SSTUserPythonFileFormat/

import sst  # Use SST Library

# Creating a sender component from element tcpGlobSync (tcpGlobSync.sender) named "Sender Zero."
sender_zero = sst.Component("Sender Zero", "tcpGlobSync.sender")
sender_zero.addParams(
    {
        "tickFreq": "1s",  # frequency component updates at.
        "low_send_rate": "10",  # minimum send rate of sender and what it will limit its send rate to.
        "max_send_rate": "50",  # maximum send rate of sender
        "verbose_level": "1",  # level of console verbosity.
        "node_id": "0",  # id of node.
        "starting_cycle": "1",  # cycle in which component begins to send packets.
    }
)

sender_one = sst.Component("Sender One", "tcpGlobSync.sender")
sender_one.addParams(
    {
        "tickFreq": "1s",
        "low_send_rate": "10",
        "max_send_rate": "50",
        "verbose_level": "1",
        "node_id": "1",
        "starting_cycle": "5",
    }
)

sender_two = sst.Component("Sender Two", "tcpGlobSync.sender")
sender_two.addParams(
    {
        "tickFreq": "1s",
        "low_send_rate": "10",
        "max_send_rate": "50",
        "verbose_level": "1",
        "node_id": "2",
        "starting_cycle": "100",
    }
)

receiver = sst.Component("Receiver Node", "tcpGlobSync.receiver")
receiver.addParams(
    {
        "tickFreq": "1s",  # frequency component updates at.
        "process_rate": "100",  # number of packets consumed per tick.
        "queue_size": "200",  # size of queue.
        "verbose_level": "1",  # level of console verbosity.
        "num_nodes": "3",  # number of nodes that will connect to receiver.
        "window_size": "40000",  # window size (in sim time) for global sync detection.
        # Appears to be that 1000 cycles in sim time is 10 seconds if component tick frequency is 1s
    }
)

# Connect the senders to one of the receiver's ports.
sst.Link("Link_Zero").connect((sender_zero, "port", "1ms"), (receiver, "port0", "1ms"))
sst.Link("Link_One").connect((sender_one, "port", "1ms"), (receiver, "port1", "1ms"))
sst.Link("Link_Two").connect((sender_two, "port", "1ms"), (receiver, "port2", "1ms"))
