## Chebyshev filtering speedup bar chart (complex GEP).
set linetype 1 lc rgb "#d62728" pt 6 ps 1 lw 3
set linetype 2 lc rgb "#1f77b4" pt 6 ps 1 lw 3
set linetype 3 lc rgb "#ff7f0e" pt 6 ps 1 lw 3
set linetype 4 lc rgb "#2ca02c" pt 6 ps 1 lw 3
set linetype 5 lc rgb "#9467bd" pt 6 ps 1 lw 3
set linetype 6 lc rgb "#e377c2" pt 6 ps 1 lw 3
set linetype 7 lc rgb "#17becf" pt 6 ps 1 lw 3
set linetype 8 lc rgb "#bcbd22" pt 6 ps 1 lw 3
set linetype 9 lc rgb "#8c564b" pt 6 ps 1 lw 3
set linetype 10 lc rgb "black"  pt 6 ps 1 lw 3
set style line 11 lc rgb '#808080' lt 10 lw 1
set border back ls 11 lw 1
set tics nomirror
set style line 12 lc rgb "black" lt 0 lw 1
set grid back ls 12
set tics textcolor rgb "black"
set key right top offset 0,.0
set term pdfcairo enhanced font "Serif,22" size 16,4.5
set ylabel 'Speedup w.r.t. ChFSI FP64' offset 1.9
set yrange [0:3.6]
set xrange [-0.5:2.7]

set output "fig10.pdf"
set style data histogram
set style histogram cluster gap 4
set boxwidth 1.0
set style fill solid
set style fill pattern border -1
set xtics format ""
set grid ytics
set grid noxtics

set title "m=1728000\n n=3000 \n 2 nodes Aurora" offset -10,-4.1
set multiplot layout 1,3 margins 0.1,0.9,0.1,0.9 spacing 0.005,0.005 columnsfirst
plot "perfFiltBCCMO.dat" using 2:xtic(1) fillstyle pattern 2 fillcolor rgb "#d62728" title "FP32" , \
     "" using 3 fillstyle pattern 2 fillcolor rgb "#1f77b4" title "TF32", \
     "" using 4 fillstyle pattern 2 fillcolor rgb "#ff7f0e" title "TF32B"

set tics nomirror

set tics textcolor rgb "black"
set grid ytics
set title "m=25672375\n n=7000 \n 20 nodes Aurora" offset -10,-4.1
set xtics
set format y ''
unset ylabel

plot "perfFiltSi.dat" using 2:xtic(1) fillstyle pattern 2 fillcolor rgb "#d62728" title "FP32" , \
     "" using 3 fillstyle pattern 2 fillcolor rgb "#1f77b4" title "TF32", \
     "" using 4 fillstyle pattern 2 fillcolor rgb "#ff7f0e" title "TF32B"
set format y ''
unset ylabel
set title "m=85766121\n n=13500 \n 100 nodes Aurora" offset -10,-4.1
plot "perfFiltC.dat" using 2:xtic(1) fillstyle pattern 2 fillcolor rgb "#d62728" title "FP32" , \
     "" using 3 fillstyle pattern 2 fillcolor rgb "#1f77b4" title "TF32", \
     "" using 4 fillstyle pattern 2 fillcolor rgb "#ff7f0e" title "TF32B"
unset multiplot
unset output
