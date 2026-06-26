#!/bin/bash
#SBATCH --job-name=cf_mag_MD
#SBATCH --output=result_%j.log
#SBATCH --error=slurm_%j.log
#SBATCH --nodes=1
#SBATCH --ntasks=128
#SBATCH --cpus-per-task=1
#SBATCH --mem=8G
#SBATCH --time=24:00:00
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=alden.yorba@yale.edu

# 1. Clean environmenst and load required software modules
module purge
module load snakemake

# 2. Activate virtual environments if necessary
source ./.venv/bin/activate

# 3. Execution command
snakemake -s collapse_2.smk --cores 128