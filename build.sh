#!/bin/bash

#SBATCH --job-name=setup
#SBATCH --output=setup.log
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=4G
#SBATCH --time=10:00
#SBATCH --mail-type=ALL
#SBATCH --mail-user=alden.yorba@yale.edu

mkdir -p input output logs bin workflow_state

module reset
module load python

# Create the venv and install packages
python -m venv ./.venv
source ./.venv/bin/activate
pip install -r requirements.txt

# Build polymer
make

# Generate initial RW polymers
python models.py CRW 100 100-100 input/