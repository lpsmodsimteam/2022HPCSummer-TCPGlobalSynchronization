# TCP Global Synchronization Example

## Prerequisites
```
git clone https://github.com/lpsmodsim/2022HPCSummer-TCPGlobalSynchronization.git
```

# Demonstrate global synchronization
Use the following python driver file.

Copy to 2022HPCSummer-TCPGlobalSynchronization/tests/tcpGlobSync.py
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
Use the following python driver file.

Copy to 2022HPCSummer-TCPGlobalSynchronization/tests/tcpGlobSync.py
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
        "delayFreq": "1ms", # frequency of delay inbetween packets sent out.
        "low_send_rate": "10",  # minimum send rate of sender and what it will limit its send rate to.
        "max_send_rate": "50",  # maximum send rate of sender.
        "verbose_level": "1",  # level of console verbosity.
        "node_id": "0",  # id of node.
        "starting_cycle": "1",  # cycle in which component begins to send packets.
    }
)

sender_one = sst.Component("Sender One", "tcpGlobSync.sender")
sender_one.addParams(
    {
        "tickFreq": "1s",
        "delayFreq": "1ms",
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
        "delayFreq": "1ms",
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
The window_size parameter for the receiver has increased drastically. This is to force false positives in the global sychronization detection.

The run_time parameter for the receiver has increased drastically. This is to collect more data to better display how detection handles false positives.

The enable_pred parameter is set to 1 for the receiver. This removes the 'tail drop' dropping policy that causes global sychronization to occur.

# Running
This is assuming the user is on a system running a Ubuntu-Based Linux Distro.

Prerequisites
```
sudo apt install singularity black mypi
git clone https://github.com/tactcomplabs/sst-containers.git
```
Follow the instructions in the git repo to build the container "sstpackage-11.1.0-ubuntu-20.04.sif".
```
cp sst-containers/singularity/sstpackage-11.1.0-ubuntu-20.04.sif /usr/local/bin/
git clone https://github.com/lpsmodsim/2022HPCSummer-SST.git
sudo ./2022HPCSummer-SST/additions.def.sh
```

Running the model
```
cd 2022HPCSummer-TCPGlobalSynchronization/
make
```

Re-run the model
```
make clean
make
```

Simulation output is generated in 2022HPCSummer-TCPGlobalSynchronization/output

# Plotting

Install gnuplot
```
sudo apt install gnuplot
```

Plot the output data using the provided example script
```
gnuplot -c example-plot.gp 3
```

## Testing synchronization
You will see the following output in 2022HPCSummer-TCPGlobalSynchronization/ (click to expand):

plot-rate.png

<img src ="https://raw.githubusercontent.com/lpsmodsim/2022HPCSummer-TCPGlobalSynchronization/main/example_plots/plot-rate-sync.png" width="720">

plot-point.png

<img src="https://raw.githubusercontent.com/lpsmodsim/2022HPCSummer-TCPGlobalSynchronization/main/example_plots/plot-point-sync.png" width="720"/>

plot-avg.png

<img src="https://raw.githubusercontent.com/lpsmodsim/2022HPCSummer-TCPGlobalSynchronization/main/example_plots/plot-avg-sync.png" width="720"/>

## Testing no synchronization
You should see the following output (click to expand):

plot-rate.png

<img src ="https://raw.githubusercontent.com/lpsmodsim/2022HPCSummer-TCPGlobalSynchronization/main/example_plots/plot-rate-false.png" width="720">

plot-point.png

<img src="https://raw.githubusercontent.com/lpsmodsim/2022HPCSummer-TCPGlobalSynchronization/main/example_plots/plot-point-false.png" width="720"/>

plot-avg.png

<img src="https://raw.githubusercontent.com/lpsmodsim/2022HPCSummer-TCPGlobalSynchronization/main/example_plots/plot-avg-false.png" width="720"/>

# Generate Documentation

```
sudo apt install doxygen
cd 2022HPCSummer-TCPGlobalSynchronization
doxygen doxygen-conf
```

Doxygen documentation will be generated in 2022HPCSummer-TCPGlobalSynchronization/html 

It can be accessed at 2022HPCSummer-TCPGlobalSynchronization/html/index.html