# CF_MAGS = ["0.001", "0.0032", "0.01", "0.032", "0.1", "0.32"]
CF_MAGS = [".01"]
DTS = ["1e-3"]
INITIAL_TEMPS = ["1e-2"]
DAMPINGS = [".01"]
INPUT_DIR = "/home/accts/ajy27/ohernlab/input"
OUTPUT_DIR = "/home/accts/ajy27/ohernlab/output"

found = glob_wildcards(
    f"{INPUT_DIR}/rw_len_{polymer_len}_no_{polymer_number}.txt"
)

INPUT_IDS = list(zip(found.polymer_len, found.polymer_number))

rule all:
    input:
            expand("{out_dir}/cf_mag_{cf_mag}/temp_{temp}/rw_len_{polymer_len}_no_{polymer_number}_final_config.xyzr", out_dir=OUTPUT_DIR, cf_mag=CF_MAGS, temp=INITIAL_TEMPS, polymer_len=)
            for cf_mag in CF_MAGS
            for polymer_len, polymer_number in INPUT_IDS

rule collapse_polymer:
    input:
        "../../input/rw_len_{polymer_len}_no_{polymer_number}.txt"
    output:
        "../../output/{cf_mag}/rw_len_{polymer_len}_no_{polymer_number}_final_config.xyzr"
    params:
        out_prefix=lambda wc: (
            f"../../output/{wc.cf_mag}/rw_len_{wc.polymer_len}_no_{wc.polymer_number}_"
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
            --in ../../input/ \
            --infile "rw_len_{wildcards.polymer_len}_no_{wildcards.polymer_number}.txt" \
            --out "{params.out_prefix}" \
            --cf-mag "{wildcards.cf_mag}"
        """

"""        polymer 
        --simtype collapse_polymer 
        --damping .1 
        --dt .0001 
        --initial-temp 1e-6 
        --writestep 100000 
        --in ../../input/ 
        --infile  rw_len_101_no_34.txt 
        --out ../../output/.1/test 
        --cf-mag .1"""
