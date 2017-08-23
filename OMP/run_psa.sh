#!/bin/sh
#
#PBS -N parallel
#PBS -l walltime=15:00
#PBS -l nodes=1:r652:ppn=40
#PBS -q mei

#mudar o r641 para a maq certa
#n compilar aqui dentro

#cd ~/Trabalho1
gcc -O3 -fopenmp -o PSA PSA.c -lm
out1="output_par_file1_threads_"
out2="output_par_file2_threads_"
out3="output_par_file3_threads_"
out4="output_par_file3_threads_"

threads=( 2 )

for i in `seq 0 6`
do
	
	#export OMP_NUM_THREADS=$threads
	#aux="$out1$threads.txt"
	#for j in `seq 1 50`
	#do
	#	./PSA $aux < $1
	#done
done

#for i in `seq 1 40`
#do
#	export OMP_NUM_THREADS=$i
#	$aux="$out2$i.txt"
#	for j in `seq 1 100`
#	do
#		./PSA $aux < $2
#	done
#done

#for i in `seq 1 40`
#do
#	export OMP_NUM_THREADS=$i
#	$aux="$out2$i.txt"
#	for j in `seq 1 100`
#	do
#		./PSA $aux < $2
#	done
#done

# 2 4 8 10 16 20 32 40 threads
#a cima de 20 pode n ter escalabilidade pq n vai ter cores fisicos

