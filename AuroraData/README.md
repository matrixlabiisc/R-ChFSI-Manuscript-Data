# Aurora Benchmark Data for R-ChFSI

Benchmark data for the **Residual Chebyshev-Filtered Subspace Iteration (R-ChFSI)** method,
collected on the ALCF **Aurora** supercomputer (Intel Data Center GPU Max Series).

These results support the numerical experiments in **Section 5.2** of the paper:

> *R-ChFSI: Residual Chebyshev-Filtered Subspace Iteration for
> Large-Scale Eigenvalue Problems on GPUs*

## Directory layout

```
rchfsiAuroraDataBak/
├── real/                        # Real-valued generalized eigenproblems
│   ├── bccmo/6x6x6/            # BCC Molybdenum (m=1,728,000, n=3,000, 2 nodes)
│   ├── silicon/12x12x12/        # Silicon        (m=25,672,375, n=7,000, 20 nodes)
│   ├── diamond/15x15x15/        # Diamond        (m=85,766,121, n=13,500, 100 nodes)
│   ├── collect.sh               # Extract ChFSI / R-ChFSI residual data
│   ├── collectMP.sh             # Extract mixed-precision residual data
│   ├── plot.plt                 # → fig4.pdf / fig8.pdf  (convergence)
│   ├── plotMP.plt               # → fig5.pdf / fig9.pdf  (mixed-precision)
│   ├── plotPerf.plt             # → fig6.pdf / fig10.pdf (filtering speedups)
│   ├── plotPerfTot.plt          # → fig7.pdf / fig11.pdf (total speedups)
│   └── perfFilt*.dat, perfTot*.dat  # Performance speedup data
├── complex/                     # Complex-valued generalized eigenproblems
│   └── (same structure as real/)
└── rchfsiAuroraResponses/       # Lightweight copy for reviewer responses
    └── (processed data, scripts, and figures only — no raw .out files)
```

## Benchmark systems

All calculations use **DFT-FE** with ONCV pseudopotentials and GGA-PBE exchange-correlation.
Each job runs a single self-consistent field (SCF) iteration to isolate eigensolver behavior.

| Material | Supercell | DOFs (*m*) | Wavefunctions (*n*) | Aurora nodes |
|----------|-----------|-----------|---------------------|-------------|
| BCC Mo   | 6×6×6     | 1,728,000 | 3,000               | 2           |
| Silicon  | 12×12×12  | 25,672,375| 7,000               | 20          |
| Diamond  | 15×15×15  | 85,766,121| 13,500              | 100         |

## Method variants compared

Chebyshev polynomial degrees: *p* = 20, 40, 60, 80, 100, 120

| Label   | Low-prec filtering | R-ChFSI | MPI comm |
|---------|--------------------|---------|----------|
| ChFSI   | FP64               | No      | FP64     |
| R-ChFSI | FP64               | Yes     | FP64     |
| FP32    | FP32               | Yes     | FP64     |
| TF32    | TF32               | Yes     | FP64     |
| TF32B   | TF32               | Yes     | BF16     |

## Per-material directory contents

Each material directory (e.g. `real/bccmo/6x6x6/`) contains:

| File(s) | Description |
|---------|-------------|
| `parameterFile.prm` | DFT-FE parameter file |
| `coordinates.inp`   | Atomic positions (fractional) |
| `domainVectors.inp` | Unit cell vectors (Bohr) |
| `pseudo.inp`        | Pseudopotential mapping |
| `*.upf`             | ONCV pseudopotential files |
| `job.sh`            | PBS job script for Aurora |
| `filtTimes.sh`      | Extract average Chebyshev filtering wall times |
| `totTimes.sh`       | Extract average total SCF iteration wall times |
| `*.out`             | Raw DFT-FE output files |
| `chfsi.dat`         | ChFSI residual convergence data |
| `rchfsi.dat`        | R-ChFSI (FP64) residual convergence data |
| `rchfsiFP32.dat`    | R-ChFSI (FP32) residual convergence data |
| `rchfsiTF32.dat`    | R-ChFSI (TF32) residual convergence data |
| `rchfsiTF32_b.dat`  | R-ChFSI (TF32 + BF16 comm) residual convergence data |

### Output file naming convention

```
{wfc}_{chebP}_{blkSize}_{opType}_{spCheb}_{rchfsi}_{mpsr}_{commType}.out
```
Example: `3600_60_900_TF32_true_true_true_BF16.out`

### Residual data format (`.dat`)

Tab-separated, 7 columns:
```
iteration  p=20  p=40  p=60  p=80  p=100  p=120
```
Values are `max_j r_j^{(i)}` (maximum residual norm over all eigenstates).
`NaN` indicates the run converged before that iteration.

### Performance data format (`perfFilt*.dat`, `perfTot*.dat`)

Tab-separated, 4 columns:
```
"p=<degree>"  FP32_speedup  TF32_speedup  TF32B_speedup
```
Speedups are relative to ChFSI FP64 baseline.

## Reproducing the figures

### Residual convergence plots (Figs. 4, 5, 8, 9 in paper)

```bash
cd real/    && gnuplot plot.plt       # → fig4.pdf  (Fig. 4)
cd real/    && gnuplot plotMP.plt     # → fig5.pdf  (Fig. 5)
cd complex/ && gnuplot plot.plt       # → fig8.pdf  (Fig. 8)
cd complex/ && gnuplot plotMP.plt     # → fig9.pdf  (Fig. 9)
```

### Performance speedup bar charts (Figs. 6, 7, 10, 11 in paper)

```bash
cd real/    && gnuplot plotPerf.plt    # → fig6.pdf  (Fig. 6)
cd real/    && gnuplot plotPerfTot.plt # → fig7.pdf  (Fig. 7)
cd complex/ && gnuplot plotPerf.plt    # → fig10.pdf (Fig. 10)
cd complex/ && gnuplot plotPerfTot.plt # → fig11.pdf (Fig. 11)
```

### Regenerating processed data from raw outputs

```bash
cd real/    && bash collect.sh       # → chfsi.dat, rchfsi.dat per material
cd real/    && bash collectMP.sh     # → rchfsiFP32.dat, rchfsiTF32.dat, rchfsiTF32_b.dat
cd complex/ && bash collect.sh
cd complex/ && bash collectMP.sh
```

## Hardware

- **System**: ALCF Aurora, Argonne National Laboratory
- **Nodes**: HPE Cray EX with Intel Xeon Max 9470C (2 sockets, 52 cores/socket)
- **GPUs**: 6 × Intel Data Center GPU Max Series (PVC) per node
- **Interconnect**: HPE Slingshot 11 (dragonfly topology)
- **MPI configuration**: 12 ranks/node, 8 OpenMP threads/rank, 6 GPUs/node
