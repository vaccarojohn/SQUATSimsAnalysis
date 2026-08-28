#!/bin/bash

#SBATCH --partition=torino
#SBATCH --account=supercdms:g4cmp
#SBATCH --job-name=g4cmp
#SBATCH --output=logs/output-%j.txt
#SBATCH --error=logs/output-%j.txt
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --mem-per-cpu=10g

CONTAINER_PATH=/sdf/data/supercdms/workarea/G4CMP-project/soft/containers/G4CMP_V10-04-00.sif
G4CMP_VERSION="V10-04-00"
FRIDGE="OLAF"
G4MACRO=/sdf/home/j/jvaccaro/DMQIS/DMQIS-OLAF-G4CMP/batch/g4macros/cs137source_batch.mac

COMMAND="/sdf/home/j/jvaccaro/DMQIS/${FRIDGE}_build/${FRIDGE} ${G4MACRO}"
echo $COMMAND
apptainer exec ${CONTAINER_PATH} ${COMMAND}
