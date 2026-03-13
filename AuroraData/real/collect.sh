#!/bin/bash
# Collect ChFSI and R-ChFSI residual convergence data from DFT-FE outputs.
# Produces chfsi.dat and rchfsi.dat for each material (BCC Mo, Diamond, Silicon).
# Output format: iteration p=20 p=40 p=60 p=80 p=100 p=120 (tab-separated).
cd /home/nikhilk/rchfsiAuroraData/real/bccmo/6x6x6
mkdir -p cols
grep "Maximum residual norm a" *_20_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col1.txt"
grep "Maximum residual norm a" *_40_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col2.txt"
grep "Maximum residual norm a" *_60_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col3.txt"
grep "Maximum residual norm a" *_80_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col4.txt"
grep "Maximum residual norm a" *_100_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col5.txt"
grep "Maximum residual norm a" *_120_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col6.txt"
# paste with tab delimiter, preserve empty fields, add NR as first column,
# and convert empty fields to NaN so gnuplot will skip them
paste -d $'\t' cols/*.txt | awk -F'\t' 'BEGIN{OFS="\t"}{
  printf "%d", NR;
  for(i=1;i<=NF;i++){
    if($i=="" || $i~/^[[:space:]]*$/) $i="NaN";
    printf "\t%s", $i;
  }
  print "";
}' > chfsi.dat

echo "Wrote chfsi.dat with $(awk "NR==1{print NF}" chfsi.dat) columns (including x)."
rm -r cols
mkdir -p cols
grep "Maximum residual norm a" *_20_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col1.txt"
grep "Maximum residual norm a" *_40_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col2.txt"
grep "Maximum residual norm a" *_60_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col3.txt"
grep "Maximum residual norm a" *_80_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col4.txt"
grep "Maximum residual norm a" *_100_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col5.txt"
grep "Maximum residual norm a" *_120_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col6.txt"
# paste with tab delimiter, preserve empty fields, add NR as first column,
# and convert empty fields to NaN so gnuplot will skip them
paste -d $'\t' cols/*.txt | awk -F'\t' 'BEGIN{OFS="\t"}{
  printf "%d", NR;
  for(i=1;i<=NF;i++){
    if($i=="" || $i~/^[[:space:]]*$/) $i="NaN";
    printf "\t%s", $i;
  }
  print "";
}' > rchfsi.dat

echo "Wrote rchfsi.dat with $(awk "NR==1{print NF}" rchfsi.dat) columns (including x)."
rm -r cols
cd /home/nikhilk/rchfsiAuroraData/real/diamond/15x15x15
mkdir -p cols
grep "Maximum residual norm a" *_20_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col1.txt"
grep "Maximum residual norm a" *_40_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col2.txt"
grep "Maximum residual norm a" *_60_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col3.txt"
grep "Maximum residual norm a" *_80_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col4.txt"
grep "Maximum residual norm a" *_100_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col5.txt"
grep "Maximum residual norm a" *_120_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col6.txt"
# paste with tab delimiter, preserve empty fields, add NR as first column,
# and convert empty fields to NaN so gnuplot will skip them
paste -d $'\t' cols/*.txt | awk -F'\t' 'BEGIN{OFS="\t"}{
  printf "%d", NR;
  for(i=1;i<=NF;i++){
    if($i=="" || $i~/^[[:space:]]*$/) $i="NaN";
    printf "\t%s", $i;
  }
  print "";
}' > chfsi.dat

echo "Wrote chfsi.dat with $(awk "NR==1{print NF}" chfsi.dat) columns (including x)."
rm -r cols
mkdir -p cols
grep "Maximum residual norm a" *_20_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col1.txt"
grep "Maximum residual norm a" *_40_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col2.txt"
grep "Maximum residual norm a" *_60_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col3.txt"
grep "Maximum residual norm a" *_80_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col4.txt"
grep "Maximum residual norm a" *_100_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col5.txt"
grep "Maximum residual norm a" *_120_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col6.txt"
# paste with tab delimiter, preserve empty fields, add NR as first column,
# and convert empty fields to NaN so gnuplot will skip them
paste -d $'\t' cols/*.txt | awk -F'\t' 'BEGIN{OFS="\t"}{
  printf "%d", NR;
  for(i=1;i<=NF;i++){
    if($i=="" || $i~/^[[:space:]]*$/) $i="NaN";
    printf "\t%s", $i;
  }
  print "";
}' > rchfsi.dat

echo "Wrote rchfsi.dat with $(awk "NR==1{print NF}" rchfsi.dat) columns (including x)."
rm -r cols
cd /home/nikhilk/rchfsiAuroraData/real/silicon/12x12x12
mkdir -p cols
grep "Maximum residual norm a" *_20_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col1.txt"
grep "Maximum residual norm a" *_40_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col2.txt"
grep "Maximum residual norm a" *_60_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col3.txt"
grep "Maximum residual norm a" *_80_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col4.txt"
grep "Maximum residual norm a" *_100_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col5.txt"
grep "Maximum residual norm a" *_120_*_FP32_false_false_true_STANDARD.out | awk '{print $NF}' > "cols/col6.txt"
# paste with tab delimiter, preserve empty fields, add NR as first column,
# and convert empty fields to NaN so gnuplot will skip them
paste -d $'\t' cols/*.txt | awk -F'\t' 'BEGIN{OFS="\t"}{
  printf "%d", NR;
  for(i=1;i<=NF;i++){
    if($i=="" || $i~/^[[:space:]]*$/) $i="NaN";
    printf "\t%s", $i;
  }
  print "";
}' > chfsi.dat

echo "Wrote chfsi.dat with $(awk "NR==1{print NF}" chfsi.dat) columns (including x)."
rm -r cols
mkdir -p cols
grep "Maximum residual norm a" *_20_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col1.txt"
grep "Maximum residual norm a" *_40_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col2.txt"
grep "Maximum residual norm a" *_60_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col3.txt"
grep "Maximum residual norm a" *_80_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col4.txt"
grep "Maximum residual norm a" *_100_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col5.txt"
grep "Maximum residual norm a" *_120_*_FP32_false_true_true_STANDARD.out | awk '{print $NF}' > "cols/col6.txt"
# paste with tab delimiter, preserve empty fields, add NR as first column,
# and convert empty fields to NaN so gnuplot will skip them
paste -d $'\t' cols/*.txt | awk -F'\t' 'BEGIN{OFS="\t"}{
  printf "%d", NR;
  for(i=1;i<=NF;i++){
    if($i=="" || $i~/^[[:space:]]*$/) $i="NaN";
    printf "\t%s", $i;
  }
  print "";
}' > rchfsi.dat

echo "Wrote rchfsi.dat with $(awk "NR==1{print NF}" rchfsi.dat) columns (including x)."
rm -r cols

