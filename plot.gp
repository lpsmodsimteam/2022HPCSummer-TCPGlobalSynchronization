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

stats "output/receiver_data.csv" using 1:4 nooutput # collect statistic info

set multiplot layout 3,2 title "TCP Global Synchronization"

    set title "Queue Size"
    set ylabel "Packets in Queue"
    plot "output/receiver_data.csv" using 1:2 title "Queue Size of Receiver Node" with lines lw 3 

    set title "Packet Loss"
    set ylabel "Packets Dropped"
    plot "output/receiver_data.csv" using 1:3 title "Packet Loss" with lines lw 3

    set title "Link Utilization"
    set ylabel "Link Utilization (%)"  
    plot "output/receiver_data.csv" using 1:4 title "Link Utilization" with lines lw 3
    f(x) = mean_y
    fit f(x) 'output/receiver_data.csv' using 1:4 via mean_y 
    set label 1 gprintf("Mean = %g", mean_y) at 15, GPVAL_DATA_Y_MAX+10 

    set title "Global Sync Detection"
    set ylabel "Problem Detected"
    plot "output/receiver_data.csv" using 1:5 title "Global Sync Detect" with points lw 3

    set title "Transmission Rates of Senders over time"
    set ylabel "Packets Sent per Second"
    set key at graph 1, 0.8
    plot for [i=0:(ARG1-1)] "output/sender_data".i.".csv" using 1:2 title "Sender-".i with lines lw 3

    #unset label 1 # included here because label 1 was being plotted on this graph.
    #set title "Average Queue Depth"
    #set ylabel "Average Packets in Queue"
    #plot "receiver_data.csv" using 1:6 title "Avg queue depth" with lines lw 3

unset multiplot
