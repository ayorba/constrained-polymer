import numpy as np
import scipy.optimize

import Bio.PDB as PDB
from Bio.PDB import PDBParser, MMCIFParser

import sys
import stats

def load_pdb(filename: str)->PDB.Structure.Structure:
    if filename.endswith(".pdb"):
        pdb_parser = PDBParser(QUIET=True)
        structure = pdb_parser.get_structure("p", filename)
        return structure

def load_mmcif(filename: str)->PDB.Structure.Structure:
    if filename.endswith(".cif"):
        mmcif_parser = MMCIFParser(QUIET=True)
        structure = mmcif_parser.get_structure("p", filename)
        return structure

class CRW:
    # class-level attributes: bead diameter, spring constants
    sigma = 1 # Backbone bead diameter
    U_bb = 1 # Bond length potential spring constant
    e_rep = 1 # Repulsive steric potential spring constant
    mass = 1
    rng = np.random.default_rng()
    max_attempts = 100

    def __init__(self, n: int):
        self.n = n
        stuck=None
        while True:
            # if(stuck is not None):
            #     print("stuck during CRW init, trying again...")
            coords = np.zeros((n, 3), dtype=np.float32)
            stuck = False
            for i in range(1, n):
                for _ in range(self.max_attempts):
                    dr = self.rng.standard_normal(3)
                    dr = dr / np.linalg.norm(dr) * self.sigma
                    proposed = coords[i-1] + dr
                    dists = np.linalg.norm(proposed - coords[:i-1], axis=-1)
                    if not np.any(dists < self.sigma):
                        coords[i] = proposed
                        break
                else:
                    stuck = True
                    break

            if not stuck:
                self.coords = coords - coords.mean(axis=0)
                stuck=None
                return

    def show(self, verbose: bool=False):
        print(f"CRW of length {self.n}:\n\tSigma: {self.sigma}\n\tU_bb: {self.U_bb}\n\te_rep: {self.e_rep}")
        print("Stats:")
        print(f"\tRadius of Gyration: {self.compute_rg()}")
        print(f"\tTotal Potential Energy: {self.compute_U_tot()}")
        print("Residues:")
        for i in range(self.n):
            if i<10 or verbose: 
                print(f"({self.coords[i,0]}, {self.coords[i,1]}, {self.coords[i,2]})")

    def compute_U_bond(self, sep: float):
        return np.sum(.5 * self.U_bb * (1-sep/self.sigma)**2)

    def compute_U_rep(self, sep: np.ndarray):
        sep_lt_sigma = sep[sep<self.sigma]
        return np.sum(.5 * self.e_rep * (1-sep_lt_sigma/self.sigma)**2)

    def compute_rg(self):
        return stats.rg(self.coords)

    def compute_rg_n(self, n):
        return stats.rg_n(self.coords, n)
    
    def compute_U_tot(self):
        U_tot = 0
        for i in range(self.n):
            if U_tot > 1:
                import ipdb; ipdb.set_trace()
            # bonded interactions, note for first and last, there is only one bonded interaction
            if i==0 and self.n > 1:
                U_tot += self.compute_U_bond(sep=np.linalg.norm(self.coords[i]-self.coords[i+1]))
            elif i==self.n-1:
                U_tot += self.compute_U_bond(sep=np.linalg.norm(self.coords[i]-self.coords[i-1]))
            else:
                U_tot += self.compute_U_bond(sep=np.linalg.norm(self.coords[i]-self.coords[i-1])) + self.compute_U_bond(sep=np.linalg.norm(self.coords[i]-self.coords[i+1]))
            # non-bonded
            dr = np.linalg.norm(self.coords[i]-np.delete(self.coords, i, axis=0), axis=-1)
            U_tot += self.compute_U_rep(dr)
        return U_tot / 2
    
    def write_sim_config(self, sim_config_filename: str):
        with open(sim_config_filename, "w") as sim_file:
            sim_file.write(self.get_sim_config())

    def get_sim_config(self)->str:
        atom_line_format_str = "ATOM,{},{},{},{},{},{},{}\nINTRA_RESIDUE\n"
        bond_line_format_str = "INTER_RESIDUE,{},{},{},{}\n"
        end_line_str = "END\n"
        sim_string = ""
        for i in range(self.n):
            sim_string += atom_line_format_str.format(i, i, self.coords[i][0], self.coords[i][1], self.coords[i][2], self.sigma, self.mass)
        for i in range(self.n-1):
            sim_string += bond_line_format_str.format(i, i+1, self.U_bb, self.sigma)
        sim_string += end_line_str
        return sim_string

    def collapse(self, central_force: float):
        # write to sim file
        # use subprocess to call polymer --simtype collapse_polymer --dt (? what should dt be) --damping 0
        # --initial-temp (? what should initial temp be) --in ../../input/ --infile {sim_file} 
        # --out ../../output/{sim_file} --cf-mag (? what should cf-mag be)
        pass

class BADA:
    pass

class FJSC:
    pass

class InSeq:
    pass

class MPSC:
    pass

class ModMPSC:
    pass


def norm(x: np.ndarray):
    return np.linalg.norm(x)

if __name__=="__main__":
    # call with python models.py CRW 100 40-1000
    from tqdm import tqdm
    script = sys.argv[0]
    args = sys.argv[1:]
    print(args)
    crw_list = []
    filenames = []
    if args[0]=="CRW":
        num_crw = int(args[1]) if int(args[1]) else 0
        ranges = [int(arg) for arg in args[2].split("-")]
        low = ranges[0]
        high = ranges[1]
        dir = args[3] if len(args) == 4 else "./"
        print(num_crw, ranges, low, high)
        # generate 1000 CRW polymers of length 10-1000, and record their statistics
        rng = np.random.default_rng()
        crw_list = [CRW(int(i)) for i in tqdm(list(np.floor(np.logspace(np.log10(low), np.log10(high), num_crw))))]
        print(crw_list)
    
    for crw in tqdm(crw_list):
        filename = f"crw_{crw.n}.txt"
        filenames.append(filename)
        crw.write_sim_config(f"{dir}/{filename}")

    with open(f"{dir}/filenames.txt", "w") as names_file:
        names_file.write("\n".join(filenames))

    