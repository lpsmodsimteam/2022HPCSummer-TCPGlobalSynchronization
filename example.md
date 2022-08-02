### TCP Global Synchronization Example

## Prerequisites
git clone https://github.com/lpsmodsim/2022HPCSummer-TCPGlobalSynchronization.git
cd 2022HPCSummer-TCPGlobalSynchronization/tests

# Demonstrate global synchronization
Copy the following parameters into tcpGlobSync.py to demonstrate a simulation where sychronization occurs:
```
# Reference: http://sst-simulator.org/SSTPages/SSTUserPythonFileFormat/

import sst  # Use SST Library

# REQUIREMENTS:
# If n Sender components are created, the field 'node_id' should be equal to some number 0 to n-1 and should be different for each component.
# Receiver component's field 'num_ports' must be equal to the number of Sender components created.
# i.e. if I create two sender components, one must have node_id '0' and the other node_id '1'. The receiving component will have num_ports '2'.

# Creating a sender component from element tcpGlobSync (tcpGlobSync.sender) named "Sender Zero."
sender_zero = sst.Component("Sender Zero", "tcpGlobSync.sender")

# Add parameters to sender.
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

# Create a receiver and add parameters to it.
receiver = sst.Component("Receiver Node", "tcpGlobSync.receiver")
receiver.addParams(
    {
        "tickFreq": "1s",  # frequency component updates at.
        "process_rate": "100",  # number of packets consumed per tick.
        "queue_size": "200",  # size of queue.
        "verbose_level": "1",  # level of console verbosity.
        "num_nodes": "3",  # number of nodes that will connect to receiver.
        "window_size": "100",  # window size (in sim time) for global sync detection.
        # Appears to be that 1000 cycles in sim time is 10 seconds if component tick frequency is 1s
        "run_time": "300",  # Number of cycles the simulation will run.
        "enable_pred": "0",  # enable psuedo-red dropping policy to prevent sychronization from occuring.
    }
)

# Connect the senders to one of the receiver's ports.
sst.Link("Link_Zero").connect((sender_zero, "port", "1ms"), (receiver, "port0", "1ms"))
sst.Link("Link_One").connect((sender_one, "port", "1ms"), (receiver, "port1", "1ms"))
sst.Link("Link_Two").connect((sender_two, "port", "1ms"), (receiver, "port2", "1ms"))
```

# Demonstrate global synchronization not occuring.
Copy the following parameters into tcpGlobSync.py to demonstrate a simulation where sychronization does not occur:
```
# Reference: http://sst-simulator.org/SSTPages/SSTUserPythonFileFormat/

import sst  # Use SST Library

# REQUIREMENTS:
# If n Sender components are created, the field 'node_id' should be equal to some number 0 to n-1 and should be different for each component.
# Receiver component's field 'num_ports' must be equal to the number of Sender components created.
# i.e. if I create two sender components, one must have node_id '0' and the other node_id '1'. The receiving component will have num_ports '2'.

# Creating a sender component from element tcpGlobSync (tcpGlobSync.sender) named "Sender Zero."
sender_zero = sst.Component("Sender Zero", "tcpGlobSync.sender")

# Add parameters to sender.
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

# Create a receiver and add parameters to it.
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
        "run_time": "1000",  # Number of cycles the simulation will run.
        "enable_pred": "1",  # enable psuedo-red dropping policy to prevent sychronization from occuring.
    }
)

# Connect the senders to one of the receiver's ports.
sst.Link("Link_Zero").connect((sender_zero, "port", "1ms"), (receiver, "port0", "1ms"))
sst.Link("Link_One").connect((sender_one, "port", "1ms"), (receiver, "port1", "1ms"))
sst.Link("Link_Two").connect((sender_two, "port", "1ms"), (receiver, "port2", "1ms"))
```
Notice the window_size parameter for the receiver has increased drastically. This is to force false positives in the global sychronization detection.

# Running

```
make
```

Simulation output is generated in 2022HPCSummer-TCPGlobalSynchronization/output

# Plotting

Install gnuplot

```
gnuplot -c plot.gp 3
```