#!/bin/bash
# PBS job script for R-ChFSI benchmarks on ALCF Aurora.
# Runs DFT-FE with parameterized Chebyshev filtering options.
#PBS -l select=2
#PBS -l place=scatter
#PBS -l walltime=02:00:00
#PBS -l filesystems=home
#PBS -j oe
#PBS -q prod
#PBS -A DFTCalc2
#PBS -N RCHFSI


echo "Number of Nodes Allocated      = $PBS_NUM_NODES"
echo "Number of Tasks Allocated      = $PBS_NUM_PPN"
echo "CPU on node                    = $PBS_NUM_CPUS"
module load boost

export MPIR_CVAR_ENABLE_GPU=1
cd $PBS_O_WORKDIR

NNODES=$(wc -l < "$PBS_NODEFILE")
NRANKS_PER_NODE=12
NDEPTH=1
NTHREADS=8
NTOTRANKS=$(( NNODES * NRANKS_PER_NODE ))

export OMP_NUM_THREADS=8
export OMP_PLACES=threads
export DFTFE_NUM_THREADS=8
export DEAL_II_NUM_THREADS=1
export OMP_MAX_ACTIVE_LEVELS=1
export ELPA_DEFAULT_omp_threads=8
export MKL_NUM_THREADS=8
export OMP_PROC_BIND=close
export MKL_DYNAMIC=FALSE

export GPU_BIND_SCHEME="--gpu-bind=list:0.0:0.1:1.0:1.1:2.0:2.1:3.0:3.1:4.0:4.1:5.0:5.1"
export CPU_BIND_SCHEME="--cpu-bind list:0-7,104-111:8-15,112-119:16-23,120-127:24-31,128-135:32-39,136-143:40-47,144-151:52-59,156-163:60-67,164-171:68-75,172-179:76-83,180-187:84-91,188-195:92-99,196-203"

export BASE=/lus/flare/projects/DFTCalc2/dftfeDependencies10102025/dftfe_rchfsi/install/real
lscpu
wfc=3600
chebP=120
blkSize=900
opType=FP32
spCheb=false
rchfsi=false
mpsr=true
commType=STANDARD
cp parameterFile.prm parameterFile_cheb_"$chebP".prm
sed -i "59s/.*/      set NUMBER OF KOHN-SHAM WAVEFUNCTIONS = $wfc/" parameterFile_cheb_"$chebP".prm
sed -i "60s/.*/      set CHEBYSHEV POLYNOMIAL DEGREE=$chebP/" parameterFile_cheb_"$chebP".prm
sed -i "63s/.*/      set CHEBY WFC BLOCK SIZE=$blkSize/" parameterFile_cheb_"$chebP".prm
sed -i "64s/.*/      set WFC BLOCK SIZE=$blkSize/" parameterFile_cheb_"$chebP".prm
sed -i "65s/.*/      set TENSOR OP TYPE SINGLE PREC CHEBY=$opType/" parameterFile_cheb_"$chebP".prm
sed -i "66s/.*/      set USE SINGLE PREC CHEBY=$spCheb/" parameterFile_cheb_"$chebP".prm
sed -i "67s/.*/      set USE RESIDUAL CHFSI=$rchfsi/" parameterFile_cheb_"$chebP".prm
sed -i "69s/.*/      set USE MIXED PREC RR_SR =$mpsr/" parameterFile_cheb_"$chebP".prm
sed -i "72s/.*/      set COMMUN PREC CHEBY=$commType/" parameterFile_cheb_"$chebP".prm
mpiexec -n ${NTOTRANKS} --ppn ${NRANKS_PER_NODE} ${CPU_BIND_SCHEME} ${GPU_BIND_SCHEME} $BASE/dftfe parameterFile_cheb_"$chebP".prm >"$wfc"_"$chebP"_"$blkSize"_"$opType"_"$spCheb"_"$rchfsi"_"$mpsr"_"$commType".out  
rm parameterFile_cheb_"$chebP".prm

