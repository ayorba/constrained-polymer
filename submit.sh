#!/bin/bash

#SBATCH --job-name=snakemake
#SBATCH --output=results/%j.log
#SBATCH --error=errors/%j.log
#SBATCH --nodes=1
#SBATCH --ntasks=64
#SBATCH --cpus-per-task=1
#SBATCH --mem=12G
#SBATCH --time=03:00:00
#SBATCH --mail-type=ALL
#SBATCH --mail-user=alden.yorba@yale.edu

# 1. Clean environment and load required software modules

rm -rf ./.venv

mkdir -p input output results errors logs bin workflow_state
module reset
module load Python/3.12.3-GCCcore-13.3.0

# 2. Activate virtual environment and install all packages

python -m venv ./.venv
source ./.venv/bin/activate
pip install -r requirements.txt

# 3. Build polymer

make -j 8
# polymer --help

# 4. Generate input files after cleaning input directory

INPUT_DIR = "input"
rm $INPUT_DIR/*.txt

python generate_inputs.py $INPUT_DIR

# 5. Run snakemake workflow

snakemake -s collapse.smk --cores 64
