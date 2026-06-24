#!/bin/bash

#SBATCH --job-name=setup
#SBATCH --output=setup.log
#SBATCH --error=error_setup.log
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=4G
#SBATCH --constraint=oldest
#SBATCH --time=00:10:00

mkdir -p input output logs bin workflow_state

module reset
module load python

# Create the venv and install packages
python -m venv ./.venv
source ./.venv/bin/activate
pip install -r requirements.txt

# Build polymer
make -j 4

# Generate initial RW polymers
python models.py CRW 100 100-100 input/