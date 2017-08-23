#!/bin/sh
#
#PBS -N ssa
#PBS -l walltime=05:00
#PBS -l nodes=1:r652:ppn=40
#PBS -q mei

#cd ~/Trabalho1
gcc -O3 -fopenmp -o SSA SSA.c -lm
outseq1="output_seq1.txt"
outseq2="output_seq2.txt"
outseq3="output_seq3.txt"
outseq4="output_seq3.txt"

for i in `seq 1 1`
do
	./SSA $outseq1 < $1
done

for i in `seq 1 1`
do
	./SSA $outseq2 < $2
done

for i in `seq 1 1`
do
	./SSA $outseq3 < $3
done

for i in `seq 1 1`
do
	./SSA $outseq3 < $3
done
