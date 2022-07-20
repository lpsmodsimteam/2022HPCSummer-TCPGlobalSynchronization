# To use:
#
# Run simulator once to generate receiver_data.csv and sender_data[i].csv files.
# 
# Make sure that plot.gp is in the same folder as the files mentioned above.
#
# Run gnuplot -c plot.gp [Number of sender nodes in your SST composition].
#
# i.e. I am running 3 sender nodes connected to 1 receiver node.
#      I run: gnuplot -c plot.gp 3

reset session

set datafile separator ','
set terminal png size 1920,1080
set output "plot.png"
set grid
set xlabel"Time (s)"
set offset graph 0,0.1,0.1,0
set multiplot layout 3,2 title "TCP Global Synchronization"

    set title "Queue Size"
    set ylabel "Packets in Queue"
    plot "receiver_data.csv" using 1:2 title "Queue Size" with lines 

    set title "Packet Loss"
    set ylabel "Number of dropped Packets"
    plot "receiver_data.csv" using 1:3 title "Packet Loss" with lines

    set title "Link Utilization"
    set ylabel "Link Utilization (%)"
    plot "receiver_data.csv" using 1:4 title "Link Utilization" with lines

    set title "Global Sync Detection"
    set ylabel "Behavior detected"
    plot "receiver_data.csv" using 1:5 title "Global Sync Detect" with points

    set title "Send Rates"
    set ylabel "Packets per second"
    plot for [i=0:(ARG1-1)] "sender_data".i.".csv" using 1:2 title "Node".i with lines 

    set title "Average Queue Depth"
    set ylabel "Packets in Queue"
    plot "receiver_data.csv" using 1:6 title "Avg queue depth" with lines

unset multiplot
