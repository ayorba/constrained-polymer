from itertools import product
from pathlib import Path

CF_MAGS = [".01", ".02", ".05", ".1", ".2", ".5", "1"]
DTS = ["1e-6"]
INITIAL_TEMPS = ["1e-2"]
DAMPINGS = ["1"]

INPUT_DIR = Path("/home/accts/ajy27/ohernlab/programs/constrained-polymer/input")
OUTPUT_DIR = Path("/home/accts/ajy27/ohernlab/programs/constrained-polymer/output")

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
            --writestep 1000000 \
            --in {params.input_dir:q} \
            --infile {params.filename:q} \
            --out {params.output_path:q} \
            --cf-mag {params.cf_mag:q} \
            > /dev/null 2>&1
        """