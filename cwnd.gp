set terminal png size 640,480	
set output "congestion window.png"
filenames = "flow1.txt flow0.txt"
plot for  [file in filenames] file using 1:2 title file with linespoints
exit


