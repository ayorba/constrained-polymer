from itertools import product
from pathlib import Path

CF_MAGS = ['0.001', '0.0016155980984398745', '0.0026101572156825357', '0.004216965034285822', '0.006812920690579615', '0.011006941712522098', '0.01778279410038923', '0.028729848333536655', '0.046415888336127795', '0.07498942093324558', '0.12115276586285889', '0.19573417814876615', '0.31622776601683794', '0.510896977450693', '0.825404185268019', '1.333521432163324', '2.1544346900318843', '3.480700588428413', '5.62341325190349', '9.085175756516872', '14.677992676220706', '23.71373705661655', '38.31186849557293', '61.8965818891261', '100.0']
DTS = ["1e-3"]
INITIAL_TEMPS = ["1e-6"]
DAMPINGS = [".1"]

INPUT_DIR = Path("/home/accts/ajy27/ohernlab/programs/constrained-polymer/input")
OUTPUT_DIR = Path("/home/accts/ajy27/ohernlab/programs/constrained-polymer/test")

# Use an absolute path here instead if polymer is not already in $PATH.
POLYMER = "polymer"

with (INPUT_DIR / "filenames.txt").open() as names_file:
    FILENAMES = [line.strip() for line in names_file if line.strip()]

JOBS = [
    {
        "cf_mag": cf_mag,
        "dt": dt,
        "temp": temp,
        "damping": damping,
        "filename": filename,
    }
    for cf_mag, dt, temp, damping, filename in product(
        CF_MAGS,
        DTS,
        INITIAL_TEMPS,
        DAMPINGS,
        FILENAMES,
    )
]


def get_job(wildcards):
    return JOBS[int(wildcards.job)]

def output_path(wildcards):
    job = get_job(wildcards)
    filename_stem = Path(job["filename"]).stem

    return str(
        OUTPUT_DIR
        / (
            f"{filename_stem}"
            f".cf_{job['cf_mag']}"
            f".dt_{job['dt']}"
            f".temp_{job['temp']}"
            f".damping_{job['damping']}"
        )
    )


rule all:
    input:
        expand(
            "workflow_state/collapse_polymer/{job}.done",
            job=range(len(JOBS)),
        )


rule collapse_polymer:
    output:
        touch("workflow_state/collapse_polymer/{job}.done")

    log:
        "logs/collapse_polymer/{job}.log"

    params:
        executable=POLYMER,
        input_dir=str(INPUT_DIR) + "/",
        filename=lambda wc: get_job(wc)["filename"],
        cf_mag=lambda wc: get_job(wc)["cf_mag"],
        dt=lambda wc: get_job(wc)["dt"],
        temp=lambda wc: get_job(wc)["temp"],
        damping=lambda wc: get_job(wc)["damping"],
        output_path=output_path,

    shell:
        r"""
        mkdir -p "$(dirname {output:q})" \
                 "$(dirname {log:q})" \
                 "$(dirname {params.output_path:q})"

        {params.executable:q} \
            --simtype collapse_polymer \
            --dt {params.dt:q} \
            --damping {params.damping:q} \
            --initial-temp {params.temp:q} \
            --writestep 10000 \
            --in {params.input_dir:q} \
            --infile {params.filename:q} \
            --out {params.output_path:q} \
            --cf-mag {params.cf_mag:q} \
            > {log:q} 2>&1
        """