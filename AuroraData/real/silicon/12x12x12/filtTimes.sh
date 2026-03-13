#!/bin/bash
# Extract average Chebyshev filtering wall times from DFT-FE outputs.
# Prints times for 4 configurations x 6 polynomial degrees (p=20..120).

grep "Chebyshev filtering on Device, wall time" *_20_*_FP32_false_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_40_*_FP32_false_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_60_*_FP32_false_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_80_*_FP32_false_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_100_*_FP32_false_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_120_*_FP32_false_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'

grep "Chebyshev filtering on Device, wall time" *_20_*_FP32_true_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_40_*_FP32_true_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_60_*_FP32_true_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_80_*_FP32_true_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_100_*_FP32_true_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_120_*_FP32_true_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'

grep "Chebyshev filtering on Device, wall time" *_20_*_TF32_true_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_40_*_TF32_true_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_60_*_TF32_true_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_80_*_TF32_true_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_100_*_TF32_true_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_120_*_TF32_true_true_true_STANDARD.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'


grep "Chebyshev filtering on Device, wall time" *_20_*_TF32_true_true_true_BF16.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_40_*_TF32_true_true_true_BF16.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_60_*_TF32_true_true_true_BF16.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_80_*_TF32_true_true_true_BF16.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_100_*_TF32_true_true_true_BF16.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
grep "Chebyshev filtering on Device, wall time" *_120_*_TF32_true_true_true_BF16.out   | awk 'match($0, /[0-9]+\.[0-9]+/){ sum += substr($0, RSTART, RLENGTH); n++ } END{ if(n) printf "%.6f\n", sum/n }'
