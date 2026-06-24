#! ./.venv/bin/python

CF_MAGS = [".005", ".01", ".02", ".05", ".1"]
DTS = ["1e-4"]
INITIAL_TEMPS = ["1e-2"]
DAMPINGS = [".01"]
INPUT_DIR = "/home/accts/ajy27/ohernlab/programs/constrained-polymer/input"
OUTPUT_DIR = "/home/accts/ajy27/ohernlab/programs/constrained-polymer/output"

filenames = []

with open(f"{INPUT_DIR}/filenames.txt", "r") as names_file:
    filenames = names_file.readlines() 

cmds = []

base_command = "polymer --simtype collapse_polymer --dt {} --damping {} --initial-temp {} --writestep 1000 --in {}/ --infile {} --out {} --cf-mag {}"

for cf_mag in CF_MAGS:
    for dt in DTS:
        for temp in INITIAL_TEMPS:
            for damping in DAMPINGS:
                for filename in filenames:
                    cmds.append(base_command.format(dt, damping, temp, INPUT_DIR, filename, f"{OUTPUT_DIR}/{filename[:-4]}.cf_{cf_mag}.dt_{dt}.temp_{temp}.damping_{damping}", cf_mag))

print("\n".join(cmds))



