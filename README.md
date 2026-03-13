# Manuscript Data: Residual Chebyshev-Filtered Subspace Iteration (R-ChFSI)

Data and scripts for reproducing all figures in:

> **Residual-based Chebyshev filtered subspace iteration for large-scale
> Hermitian eigenvalue problems tolerant to inexact matrix-vector products**
> *N. Kodali, K. Ramakrishnan, P. Motamarri — Computer Physics Communications*

## Contents

| Directory | Description | Figures |
|-----------|-------------|---------|
| `ToyExample/` | C++17 reference implementation (Algorithms 1–3) and convergence data for dense symmetric eigenproblems with controlled noise (Section 4.1) | Figs. 1–3 |
| `AuroraData/` | Benchmark data from ALCF Aurora supercomputer — real and complex generalized eigenproblems solved with DFT-FE (Section 5.2) | Figs. 4–11 |

## Figure Summary

| Figure | File | Description |
|--------|------|-------------|
| 1 | `ToyExample/data/fig1.pdf` | ChFSI vs R-ChFSI convergence (standard eigenproblem) |
| 2 | `ToyExample/data/fig2.pdf` | ChFSI vs R-ChFSI convergence (generalised eigenproblem) |
| 3 | `ToyExample/data/fig3.pdf` | Convergence condition (Eq. 5) diagnostic |
| 4 | `AuroraData/real/fig4.pdf` | Residual convergence (real symmetric, Aurora) |
| 5 | `AuroraData/real/fig5.pdf` | Mixed-precision residuals (real symmetric, Aurora) |
| 6 | `AuroraData/real/fig6.pdf` | Filtering step speedups (real symmetric, Aurora) |
| 7 | `AuroraData/real/fig7.pdf` | Total eigensolve speedups (real symmetric, Aurora) |
| 8 | `AuroraData/complex/fig8.pdf` | Residual convergence (complex Hermitian, Aurora) |
| 9 | `AuroraData/complex/fig9.pdf` | Mixed-precision residuals (complex Hermitian, Aurora) |
| 10 | `AuroraData/complex/fig10.pdf` | Filtering step speedups (complex Hermitian, Aurora) |
| 11 | `AuroraData/complex/fig11.pdf` | Total eigensolve speedups (complex Hermitian, Aurora) |

## Regenerating Figures

**Toy example (Figs. 1–3):**

```bash
cd ToyExample
mkdir -p build && cd build
cmake .. && make -j
./plot_convergence ../data
cd ../data && gnuplot make_paper_plots.gp
```

**Aurora benchmarks (Figs. 4–11):**

```bash
cd AuroraData/real    && gnuplot plot.plt plotMP.plt plotPerf.plt plotPerfTot.plt
cd AuroraData/complex && gnuplot plot.plt plotMP.plt plotPerf.plt plotPerfTot.plt
```

## Prerequisites

- **Toy example:** C++17 compiler, CMake ≥ 3.16, BLAS/LAPACK, gnuplot
- **Aurora data:** gnuplot (figures only; raw data from DFT-FE runs on Aurora)

See `ToyExample/README.md` and `AuroraData/README.md` for full details.
