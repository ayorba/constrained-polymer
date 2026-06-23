CF_MAGS = ["0.001", "0.0032", "0.01", "0.032", "0.1", "0.32"]

found = glob_wildcards(
    "../../input/rw_len_{polymer_len}_no_{polymer_number}.txt"
)

INPUT_IDS = list(zip(found.polymer_len, found.polymer_number))

rule all:
    input:
        [
            (
                f"../../output/{cf_mag}/"
                f"rw_len_{polymer_len}_no_{polymer_number}_final_config.xyzr"
            )
            for cf_mag in CF_MAGS
            for polymer_len, polymer_number in INPUT_IDS
        ]

rule collapse_polymer:
    input:
        "../../input/rw_len_{polymer_len}_no_{polymer_number}.txt"
    output:
        "../../output/{cf_mag}/rw_len_{polymer_len}_no_{polymer_number}_final_config.xyzr"
    params:
        out_prefix=lambda wc: (
            f"../../output/{wc.cf_mag}/"
            f"rw_len_{wc.polymer_len}_no_{wc.polymer_number}_"
        )
    wildcard_constraints:
        cf_mag=r"0\.001|0\.0032|0\.01|0\.032|0\.1|0\.32"
    threads: 1
    shell:
        r"""
        mkdir -p "$(dirname "{output}")"

        polymer \
            --simtype collapse_polymer \
            --damping 0.1 \
            --dt .0001 \
            --initial-temp 1e-6 \
            --writestep 100000 \
            --in input/ \
            --infile "rw_len_{wildcards.polymer_len}_no_{wildcards.polymer_number}.txt" \
            --out "{params.out_prefix}" \
            --cf-mag "{wildcards.cf_mag}"
        """