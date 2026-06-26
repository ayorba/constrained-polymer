from itertools import product
from pathlib import Path

CF_MAGS = ['0.001', '0.0010974987654930556', '0.0012045035402587824', '0.0013219411484660286', '0.0014508287784959402', '0.0015922827933410922', '0.001747528400007683', '0.0019179102616724887', '0.00210490414451202', '0.0023101297000831605', '0.0025353644939701114', '0.0027825594022071257', '0.0030538555088334154', '0.003351602650938841', '0.0036783797718286343', '0.004037017258596553', '0.004430621457583882', '0.004862601580065354', '0.005336699231206312', '0.005857020818056667', '0.006428073117284319', '0.007054802310718645', '0.007742636826811269', '0.008497534359086447', '0.0093260334688322', '0.010235310218990263', '0.011233240329780276', '0.012328467394420665', '0.013530477745798075', '0.01484968262254465', '0.016297508346206444', '0.01788649529057435', '0.019630406500402715', '0.021544346900318843', '0.023644894126454083', '0.025950242113997372', '0.02848035868435802', '0.03125715849688237', '0.03430469286314919', '0.037649358067924694', '0.04132012400115339', '0.04534878508128584', '0.049770235643321115', '0.05462277217684343', '0.05994842503189412', '0.06579332246575682', '0.07220809018385467', '0.07924828983539177', '0.08697490026177834', '0.09545484566618342', '0.10476157527896651', '0.11497569953977368', '0.1261856883066021', '0.1384886371393873', '0.15199110829529347', '0.1668100537200059', '0.18307382802953698', '0.2009233002565048', '0.22051307399030456', '0.24201282647943834', '0.26560877829466867', '0.29150530628251786', '0.31992671377973847', '0.35111917342151344', '0.3853528593710531', '0.4229242874389499', '0.4641588833612782', '0.5094138014816381', '0.5590810182512228', '0.6135907273413176', '0.6734150657750828', '0.7390722033525783', '0.8111308307896873', '0.8902150854450392', '0.9770099572992257', '1.0722672220103242', '1.1768119524349991', '1.291549665014884', '1.4174741629268062', '1.5556761439304723', '1.7073526474706922', '1.873817422860385', '2.0565123083486534', '2.2570197196339215', '2.4770763559917115', '2.7185882427329426', '2.9836472402833403', '3.2745491628777317', '3.5938136638046294', '3.94420605943766', '4.328761281083062', '4.750810162102798', '5.21400828799969', '5.72236765935022', '6.280291441834259', '6.892612104349702', '7.56463327554629', '8.302175681319753', '9.111627561154895', '10.0']
DTS = ["1e-2"]
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