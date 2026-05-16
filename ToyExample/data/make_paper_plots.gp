## Gnuplot script: toy convergence plots for R-ChFSI paper
## Colour palette: maximally distinct hues (no multiple shades of one colour).

# ---- House style ----
set linetype 1 lc rgb "#d62728" pt 6 ps 1 lw 3   # red
set linetype 2 lc rgb "#1f77b4" pt 6 ps 1 lw 3   # blue
set linetype 3 lc rgb "#ff7f0e" pt 6 ps 1 lw 3   # orange
set linetype 4 lc rgb "#2ca02c" pt 6 ps 1 lw 3   # green
set linetype 5 lc rgb "#9467bd" pt 6 ps 1 lw 3   # purple
set linetype 6 lc rgb "#e377c2" pt 6 ps 1 lw 3   # pink
set linetype 7 lc rgb "#17becf" pt 6 ps 1 lw 3   # cyan
set linetype 8 lc rgb "#bcbd22" pt 6 ps 1 lw 3   # olive
set linetype 9 lc rgb "#8c564b" pt 6 ps 1 lw 3   # brown
set linetype 10 lc rgb "black"  pt 6 ps 1 lw 3
set style line 11 lc rgb '#808080' lt 10 lw 1
set border back ls 11 lw 1
set tics nomirror
set style line 12 lc rgb "black" lt 0 lw 1
set grid back ls 12
set tics textcolor rgb "black"

# =====================================================================
#  Plot 1 – Standard eigenproblem: residual & subspace angle (2×4)
# =====================================================================
set term pdfcairo enhanced font "Serif,28" size 20,8
set output "fig1.pdf"

set multiplot layout 2,4 margins 0.06,0.99,0.10,0.97 spacing 0.005,0.005

set logscale y
set xrange [0:100]
set xtics 0,20,80
set key right top font ",28" samplen 2

noise_label(i) = (i==0 ? "{/Symbol e} = 0 (exact)" : \
                  i==1 ? "{/Symbol e} = 10^{-4}" : \
                  i==2 ? "{/Symbol e} = 10^{-3}" : \
                         "{/Symbol e} = 10^{-2}")

# --- top row: residuals ---
do for [i=0:3] {
    unset title
    set format x ""; unset xlabel
    if (i == 0) { set format y "10^{%T}"; set ylabel "Max residual" offset 1.5 } \
    else        { set format y ""; set ylabel "" }
    set yrange [1e-15:1e3]
    set label 1 noise_label(i) at graph 0.50, graph 0.92 center font ",28"
    set key at graph 0.97, graph 0.92
    plot \
      sprintf("conv_standard_chfsi_noise%d.dat", i)  u 1:2 w l lt 1 lw 3 title "ChFSI", \
      sprintf("conv_standard_rchfsi_noise%d.dat", i) u 1:2 w l lt 2 lw 3 title "R-ChFSI"
    unset label 1
}

# --- bottom row: subspace angles ---
do for [i=0:3] {
    unset title
    set format x "%g"; set xlabel "Iteration"
    if (i == 0) { set format y "10^{%T}"; set ylabel "sin(max angle)" offset 1.5 } \
    else        { set format y ""; set ylabel "" }
    set yrange [1e-15:1e1]
    set label 1 noise_label(i) at graph 0.50, graph 0.92 center font ",28"
    set key at graph 0.97, graph 0.92
    plot \
      sprintf("conv_standard_chfsi_noise%d.dat", i)  u 1:3 w l lt 1 lw 3 title "ChFSI", \
      sprintf("conv_standard_rchfsi_noise%d.dat", i) u 1:3 w l lt 2 lw 3 title "R-ChFSI"
    unset label 1
}

unset multiplot

# =====================================================================
#  Plot 2 – Generalized eigenproblem: residual & subspace angle (2×4)
# =====================================================================
set output "fig2.pdf"

set multiplot layout 2,4 margins 0.06,0.99,0.10,0.97 spacing 0.005,0.005

set logscale y
set xrange [0:100]
set xtics 0,20,80

zeta_label(i) = (i==0 ? "{/Symbol z} = 0 (exact)" : \
                 i==1 ? "{/Symbol z} = 10^{-4}" : \
                 i==2 ? "{/Symbol z} = 10^{-3}" : \
                        "{/Symbol z} = 10^{-2}")

# --- top row: residuals ---
do for [i=0:3] {
    unset title
    set format x ""; unset xlabel
    if (i == 0) { set format y "10^{%T}"; set ylabel "Max residual" offset 1.5 } \
    else        { set format y ""; set ylabel "" }
    set yrange [1e-15:1e3]
    set label 1 zeta_label(i) at graph 0.50, graph 0.92 center font ",28"
    if (i == 3) { set key at graph 0.97, graph 0.25 } \
    else        { set key at graph 0.97, graph 0.92 }
    plot \
      sprintf("conv_generalized_chfsi_zeta%d.dat", i)  u 1:2 w l lt 1 lw 3 title "ChFSI", \
      sprintf("conv_generalized_rchfsi_zeta%d.dat", i) u 1:2 w l lt 2 lw 3 title "R-ChFSI"
    unset label 1
}

# --- bottom row: subspace angles ---
do for [i=0:3] {
    unset title
    set format x "%g"; set xlabel "Iteration"
    if (i == 0) { set format y "10^{%T}"; set ylabel "sin(max angle)" offset 1.5 } \
    else        { set format y ""; set ylabel "" }
    set yrange [1e-15:1e1]
    set label 1 zeta_label(i) at graph 0.50, graph 0.92 center font ",28"
    if (i == 3) { set key at graph 0.97, graph 0.25 } \
    else        { set key at graph 0.97, graph 0.92 }
    plot \
      sprintf("conv_generalized_chfsi_zeta%d.dat", i)  u 1:3 w l lt 1 lw 3 title "ChFSI", \
      sprintf("conv_generalized_rchfsi_zeta%d.dat", i) u 1:3 w l lt 2 lw 3 title "R-ChFSI"
    unset label 1
}

unset multiplot

# =====================================================================
#  Plot 3 – Eq. (5) diagnostic: 1×2 (filter error / LHS vs RHS) for zeta=10^{-3}
# =====================================================================
set term pdfcairo enhanced font "Serif,22" size 16,6
set output "fig3.pdf"

set multiplot layout 1,2 margins 0.08,0.97,0.14,0.95 spacing 0.005

set xrange [1:100]
set logscale y

# Left: filter error
set format y "10^{%T}"
set format x "%g"
set xlabel "Iteration"
set ylabel "||{/Symbol D}_{p}^{(i)}||" offset 1.5
set yrange [1e-18:1e1]
set label 1 "{/Symbol z} = 10^{-3}" at graph 0.50, graph 0.92 center font ",22"
set key at graph 0.97, graph 0.92
plot \
  "conv_generalized_chfsi_zeta2.dat"  u 1:($4>0?$4:1e-20) w l lt 1 lw 3 title "ChFSI", \
  "conv_generalized_rchfsi_zeta2.dat" u 1:($4>0?$4:1e-20) w l lt 2 lw 3 title "R-ChFSI"
unset label 1

# Right: LHS vs RHS
set format y ""
set ylabel ""
set yrange [1e-4:1e5]
set label 1 "{/Symbol z} = 10^{-3}" at graph 0.50, graph 0.92 center font ",22"
set key at graph 0.97, graph 0.92
plot \
  "conv_generalized_chfsi_zeta2.dat"  u 1:($5>0?$5:1e-20) w l lt 3 lw 3 title "LHS (Cheb. gap)", \
  "conv_generalized_chfsi_zeta2.dat"  u 1:($6>0?$6:1e-20) w l lt 1 lw 3 title "ChFSI RHS", \
  "conv_generalized_rchfsi_zeta2.dat" u 1:($6>0?$6:1e-20) w l lt 2 lw 3 title "R-ChFSI RHS"
unset label 1

unset multiplot

set output
print "Done – wrote toy_convergence_standard.pdf, toy_convergence_generalized.pdf, toy_eq5_diagnostic.pdf"
