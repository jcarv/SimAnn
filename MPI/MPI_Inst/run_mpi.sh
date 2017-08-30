#!/bin/sh
#
#PBS -N mpi
#PBS -l walltime=2:00:00
#PBS -l nodes=2:r641:ppn=32
#PBS -q cpd

cd ~/Trabalho2

module load gnu/openmpi_eth/1.6.2

out1="output_mpi_file1_procs_"
out2="output_mpi_file2_procs_"

for i in 2 4 8 16 32 64
do
	$aux="$out1$i.txt"

	for j in `seq 1 1`
	do
		mpirun -np $i -mca btl self,sm,tcp -map-by core ./MPIPSA $aux < config150x10.txt
	done
done

for i in 2 4 8 16 32 64
do
	$aux="$out2$i.txt"

	for j in `seq 1 1`
	do
		mpirun -np $i -mca btl self,sm,tcp -map-by core ./MPIPSA $aux < config500x10.txt
	done
done
