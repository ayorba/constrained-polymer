from snakemake.io import glob_wildcards

PROGRAM = "polymer"

CF_MAGS = [
    "0.001",
    "0.0032",
    "0.01",
    "0.032",
    "0.1",
    "0.32",
]

INPUT_PATTERN = "../../input/rw_len_{polymer_len}_no_{polymer_number}.txt"

# Assumes --out writes exactly this file.
OUTPUT_PATTERN = (
    "../../output/{cf_mag}/"
    "rw_len_{polymer_len}_no_{polymer_number}"
)

LOG_PATTERN = (
    "../../logs/collapse/{cf_mag}/"
    "rw_len_{polymer_len}_no_{polymer_number}.log"
)

discovered = glob_wildcards(INPUT_PATTERN)

SAMPLES = list(
    zip(discovered.polymer_len, discovered.polymer_number)
)

if not SAMPLES:
    raise ValueError(f"No input files matched: {INPUT_PATTERN}")

TARGETS = [
    OUTPUT_PATTERN.format(
        cf_mag=cf_mag,
        polymer_len=polymer_len,
        polymer_number=polymer_number,
    )
    for cf_mag in CF_MAGS
    for polymer_len, polymer_number in SAMPLES
]


rule all:
    input:
        TARGETS


rule collapse_polymer:
    input:
        INPUT_PATTERN

    output:
        OUTPUT_PATTERN

    log:
        LOG_PATTERN

    params:
        # Pass only the filename because --in is input/
        infile=lambda wc: (
            f"rw_len_{wc.polymer_len}_no_{wc.polymer_number}.txt"
        )

    threads: 1

    wildcard_constraints:
        cf_mag=r"0\.001|0\.0032|0\.01|0\.032|0\.1|0\.32",
        polymer_len=r"\d+",
        polymer_number=r"\d+"

    shell:
        r"""
        mkdir -p "$(dirname {output:q})" "$(dirname {log:q})"

        {PROGRAM:q} \
            --simtype collapse_polymer \
            --damping 0.1 \
            --dt 0.0001 \
            --initial-temp 1e-6 \
            --writestep 100000 \
            --in ../../input/ \
            --infile {params.infile:q} \
            --out {output:q} \
            --cf-mag {wildcards.cf_mag:q} \
            > {log:q} 2>&1
        """