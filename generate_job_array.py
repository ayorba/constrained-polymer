#! ./.venv/bin/python

CF_MAGS = [".005", ".01", ".02", ".05", ".1"]
DTS = ["1e-4"]
INITIAL_TEMPS = ["1e-2"]
DAMPINGS = [".01"]
INPUT_DIR = "/home/accts/ajy27/ohernlab/input"
OUTPUT_DIR = "/home/accts/ajy27/ohernlab/output"

filenames = []

input_files = 

cmds = []

base_command = "polymer --simtype collapse_polymer --dt {} --damping {} --initial-temp {} --writestep 1000 --in {} --infile {} --out {} --cf-mag {}"

for cf_mag in CF_MAGS:
    for dt in DTS:
        for temp in INITIAL_TEMPS:
            for damping in DAMPINGS:
                for filename in filenames:
                    cmds.append(base_command.format(dt, damping, temp, INPUT_DIR, filename, f"{OUTPUT_DIR}/{filename[:-3]}", cf_mag))

print(cmds)

