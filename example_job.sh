#!/bin/bash

#SBATCH --job-name=example_job
#SBATCH --time=2:00:00
#SBATCH --mail-type=ALL

module reset
echo "hello"