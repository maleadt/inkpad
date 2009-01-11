# gnuplot script for 'test.dat'
set xlabel "Elements in container"
set ylabel "Time to process"

plot "test.dat" using 1:2 smooth bezier title "std"
replot "test.dat" using 1:3 smooth bezier title "ustd"
set terminal png size 1280,786
set output "Vector.png"
replot
clear

plot "test.dat" using 1:4 smooth bezier title "std"
replot "test.dat" using 1:5 smooth bezier title "ustd"
set terminal png size 1280,786
set output "List.png"
replot
clear
