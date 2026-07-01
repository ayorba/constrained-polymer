import numpy as np
from models import CRW
import os
import sys


lens = [128, 256, 512, 1024]
INPUT_DIR = sys.argv[1]

for length in lens:
    crw = CRW(length)
    crw.write_sim_config(os.path.join(INPUT_DIR, f"crw_{length}.txt"))
