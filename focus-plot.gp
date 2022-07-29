reset session

set datafile separator ','
set terminal pngcairo size 1920, 1080
set output "plot-full.png"
set grid
set xlabel "Time (s)"
set offset graph 0,0.1,0.1,0
set key font ",12"
set xtics font ",12"
set ytics font ",12"
set ylabel font ",16"
set xlabel font ",16"
set title font ",16"

set title "Transmission Rates of Senders over Time"
set ylabel "Packets Sent Per Second"
#plot "output/sender_data0.csv" using 1:2 title "Sender-0" with lines lw 4 lt rgb "#ff0067" dashtype 1, "output/sender_data1.csv" using 1:2 title "Sender-1" with lines lt rgb "#0000ff" lw 4 dashtype 1, "output/sender_data2.csv" using 1:2 title "Sender-2" with lines dashtype 1 lt rgb "#ffa600" lw 4 

set title "Global Synchronization Detection"
set ylabel "Synchronization Detected"
#plot "output/receiver_data.csv" using 1:5 title "Global Sync Detect" with points lw 8

set title "Global Synchronization Detection"
set xlabel "Number of Times Sychronized Rate Limiting is Detected."
set ylabel "Average Time Difference"
plot "output/receiver_data.csv" using 7:8 title "Average Time Difference between Detection Points" with lines lw 4

set title "Variance in Time Difference between synchronization"
set xlabel "blah"
set ylabel "blah"
#plot "output/receiver_data.csv" using 7:9 title "Variance" with lines lw 4

