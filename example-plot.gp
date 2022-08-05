reset session

set datafile separator ',' # for csv extraction.
set terminal pngcairo size 1920, 1080 # size of output files

# global settings
set grid
set xlabel "Time (Simulator Ticks)"
set offset graph 0,0.1,0.1,0
set key font ",18"
set xtics font ",18"
set ytics font ",18"
set ylabel font ",20" offset -2.5, 0
set xlabel font ",20"
set title font ",20"
set lmargin 13

path_data = "output/" # Change to where data is stored. (output/ by default)

# PLOTS FOR DATA WHERE SYCHRONIZATION OCCURS.
set output "plot-rate.png"
set title "Transmission Rates of Senders over Simulation Time"
set ylabel "Packets Sent Per Tick"
plot path_data."sender_data0.csv" using 1:2 title "Sender-0" with lines lw 5 lt rgb "#ff0067" dashtype 1, path_data."sender_data1.csv" using 1:2 title "Sender-1" with lines lt rgb "#0000ff" lw 5 dashtype 1, path_data."sender_data2.csv" using 1:2 title "Sender-2" with lines dashtype 1 lt rgb "#ffa600" lw 5

set output "plot-point.png"
set title "Global Synchronization Detection"
set ylabel "Synchronization Detected"
plot path_data."receiver_data.csv" using 1:5 title "Global Sync Detected" with points lw 10

set output "plot-avg.png"
set title "Global Synchronization Detection"
set xlabel "Number of Times Sychronized Rate Limiting is Detected."
set ylabel "Average Time Difference (ms)"
plot path_data."receiver_data.csv" using 6:7 title "Average Time Difference between Detection Points" with lines lw 5



