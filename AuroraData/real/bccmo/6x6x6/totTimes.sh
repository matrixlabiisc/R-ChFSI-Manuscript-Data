#!/bin/bash
# Extract average total SCF iteration wall times from DFT-FE outputs.
# Prints times for 4 configurations x 5 polynomial degrees (p=40..120).
grep "Wall time for the above scf iteration" *_40_*_FP32_false_true_true_STANDARD.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_60_*_FP32_false_true_true_STANDARD.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_80_*_FP32_false_true_true_STANDARD.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_100_*_FP32_false_true_true_STANDARD.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_120_*_FP32_false_true_true_STANDARD.out|awk '{print $(NF-1)}' 

grep "Wall time for the above scf iteration" *_40_*_FP32_true_true_true_STANDARD.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_60_*_FP32_true_true_true_STANDARD.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_80_*_FP32_true_true_true_STANDARD.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_100_*_FP32_true_true_true_STANDARD.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_120_*_FP32_true_true_true_STANDARD.out|awk '{print $(NF-1)}' 

grep "Wall time for the above scf iteration" *_40_*_TF32_true_true_true_STANDARD.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_60_*_TF32_true_true_true_STANDARD.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_80_*_TF32_true_true_true_STANDARD.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_100_*_TF32_true_true_true_STANDARD.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_120_*_TF32_true_true_true_STANDARD.out|awk '{print $(NF-1)}' 

grep "Wall time for the above scf iteration" *_40_*_TF32_true_true_true_BF16.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_60_*_TF32_true_true_true_BF16.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_80_*_TF32_true_true_true_BF16.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_100_*_TF32_true_true_true_BF16.out|awk '{print $(NF-1)}' 
grep "Wall time for the above scf iteration" *_120_*_TF32_true_true_true_BF16.out|awk '{print $(NF-1)}' 
