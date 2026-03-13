## Mixed-precision R-ChFSI convergence for diamond only (complex GEP).
set linetype 1 lc rgb "#8b1a0e" pt 6 ps 1 lw 3
set linetype 2 lc rgb "#4169e1" pt 6 ps 1 lw 3
set linetype 3 lc rgb "#5e9c36" pt 6 ps 1 lw 3
set linetype 4 lc rgb "#ff4500" pt 6 ps 1 lw 3
set linetype 5 lc rgb "#00008b" pt 6 ps 1 lw 3
set linetype 6 lc rgb "#006400" pt 6 ps 1 lw 3
set linetype 7 lc rgb "#8b008b" pt 6 ps 1 lw 3
set linetype 8 lc rgb "#808000" pt 6 ps 1 lw 3
set linetype 9 lc rgb "#ff00ff" pt 6 ps 1 lw 3
set linetype 10 lc rgb "black"  pt 6 ps 1 lw 3
set style line 11 lc rgb "#808080" lt 10 lw 1
set border back ls 11 lw 1
set tics nomirror
set style line 12 lc rgb "black" lt 0 lw 1
set grid back ls 12
set tics textcolor rgb "black"
set key columns 3 maxrows 6 right top offset 0,-0.6
set term pdfcairo enhanced font "Serif,22" size 16,4
set ylabel "max_{/Times-Italic j} {/Times-Italic r_j^{(i)}}" offset 1.9
set logscale y 10
set yrange [1e-9:5000]
set xrange [0:99]
set ytics (1,0.01,0.0001,1e-6,1e-8) 
eps = 0.8
unset mytics
set key samplen 3.4
set format y "10^{%T}"
set format x ""

set output "gep_residuals_complex_lp_diamond.pdf"
set multiplot layout 1,3 margins 0.05,0.99,0.15,0.99 spacing 0.005,0.005
set title "m=1728000\n n=3000" offset -16,-3.2
set format x
set xlabel "Iteration Number {/Times-Italic (i)}" offset 0,0.8


set ylabel "max_{/Times-Italic j} {/Times-Italic r_j^{(i)}}" offset 1.9
set format y "10^{%T}"
set format x
set xlabel "Iteration Number {/Times-Italic (i)}" offset 0,0.8
set title "m=85766121\n n=13500" offset -16,-3.2
plot keyentry "{p=20}",keyentry "{p=40}",keyentry "{p=60}",keyentry "{p=80}",keyentry "{p=100}",keyentry "{p=120}", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:2 w l  title "FP64", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:3 w l  title "FP64","/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:4 w l  title "FP64", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:5 w l  title "FP64", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:6 w l  title "FP64","/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:7 w l  title "FP64", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiFP32.dat" u 1:($2*eps) w l lc 1 dt 4 title "FP32", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiFP32.dat" u 1:($3*eps) w l lc 2 dt 4 title "FP32","/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiFP32.dat" u 1:($4*eps) w l lc 3 dt 4 title "FP32", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiFP32.dat" u 1:($5*eps) w l lc 4 dt 4 title "FP32", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiFP32.dat" u 1:($6*eps) w l lc 5 dt 4 title "FP32", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiFP32.dat" u 1:($7*eps) w l lc 6 dt 4 title "FP32"
unset ylabel
set format y ""
set title "m=85766121\n n=13500" offset -16,-3.2
plot keyentry "{p=20}",keyentry "{p=40}",keyentry "{p=60}",keyentry "{p=80}",keyentry "{p=100}",keyentry "{p=120}", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:2 w l  title "FP64", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:3 w l  title "FP64","/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:4 w l  title "FP64", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:5 w l  title "FP64", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:6 w l  title "FP64","/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:7 w l  title "FP64", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiTF32.dat" u 1:($2*eps) w l lc 1 dt 4 title "TF32", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiTF32.dat" u 1:($3*eps) w l lc 2 dt 4 title "TF32","/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiTF32.dat" u 1:($4*eps) w l lc 3 dt 4 title "TF32", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiTF32.dat" u 1:($5*eps) w l lc 4 dt 4 title "TF32", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiTF32.dat" u 1:($6*eps) w l lc 5 dt 4 title "TF32", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiTF32.dat" u 1:($7*eps) w l lc 6 dt 4 title "TF32"
set title "m=85766121\n n=13500" offset -16,-3.2
plot keyentry "{p=20}",keyentry "{p=40}",keyentry "{p=60}",keyentry "{p=80}",keyentry "{p=100}",keyentry "{p=120}", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:2 w l  title "FP64", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:3 w l  title "FP64","/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:4 w l  title "FP64", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:5 w l  title "FP64", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:6 w l  title "FP64","/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsi.dat" u 1:7 w l  title "FP64", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiTF32_b.dat" u 1:($2*eps) w l lc 1 dt 4 title "TF32B", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiTF32_b.dat" u 1:($3*eps) w l lc 2 dt 4 title "TF32B","/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiTF32_b.dat" u 1:($4*eps) w l lc 3 dt 4 title "TF32B", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiTF32_b.dat" u 1:($5*eps) w l lc 4 dt 4 title "TF32B", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiTF32_b.dat" u 1:($6*eps) w l lc 5 dt 4 title "TF32B", "/home/nikhilk/rchfsiAuroraData/complex/diamond/15x15x15/rchfsiTF32_b.dat" u 1:($7*eps) w l lc 6 dt 4 title "TF32B"

unset multiplot
unset output
