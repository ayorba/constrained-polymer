import numpy as np
import scipy.optimize

import Bio.PDB as PDB
from Bio.PDB import PDBParser, MMCIFParser

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
        self.res_list = []
        while True:
            coords = np.zeros((n, 3), dtype=np.float32)
            stuck = False
            for i in range(1, n):
                for _ in range(self.max_attempts):
                    dr = self.rng.standard_normal(3)
                    dr /= np.linalg.norm(dr)
                    proposed = coords[i-1] + dr
                    dists = np.linalg.norm(proposed - coords[:i-1], axis=-1)
                    if not np.any(dists < self.sigma):
                        coords[i] = proposed
                        break
                else:
                    stuck = True
                    break
            if not stuck:
                self.coords = coords
                return

    def init_old(self, n: int):
        self.coords = np.zeros(shape=(n, 3), dtype=np.float32) # list of backbone coordinates
        self.res_list = [] # list of residues
        self.n = n
        # initialize coords of each backbone bead
        # after the second bead, minimize potential energy w/ each new bead
        for i in range(self.n):
            if i==0:
                continue
            dr = self.rng.standard_normal(size=(3,))
            dr = dr / np.linalg.norm(dr)
            # current position is coords[i-1]+dr, while U_rep bewteen coords[i] and all previous beads (not including the one it's bonded to) 
            max_tries=5
            count=0
            while(self.compute_U_rep(np.sqrt(np.sum((self.coords[i-1]+dr - self.coords[:i-1])**2, axis=-1))) > 0 and count < max_tries):
                dr = self.rng.standard_normal(size=(3,))
                dr = dr / np.linalg.norm(dr)
            if count==max_tries:
                print("stuck generating initial config for CRW, exiting...")
                exit(0)
            self.coords[i]=self.coords[i-1]+dr
            # res = scipy.optimize.minimize(fun=lambda r_i: self.compute_U_bond(sep=np.sqrt(np.sum((r_i-self.coords[i-1])**2))) + self.compute_U_rep(np.sqrt(np.sum((r_i - self.coords[:i-1])**2, axis=-1))), x0=self.coords[i-1]+dr)
            # print(f"({res.x[0]}, {res.x[1]}, {res.x[2]})")
            # self.coords[i]=res.x

    def show(self, verbose: bool=False):
        print(f"CRW of length {self.n}:\n\tSigma: {self.sigma}\n\tU_bb: {self.U_bb}\n\te_rep: {self.e_rep}")
        print("Stats:")
        print(f"\tRadius of Gyration: {self.compute_rg()}")
        print(f"\tTotal Potential Energy: {self.compute_U_tot()}")
        print("Residues:")
        for i in range(self.n):
            if i<10 or verbose: 
                print(f"\tBead {i}: ({self.coords[i,0]}, {self.coords[i,1]}, {self.coords[i,2]})")

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
            # bonded interactions, note for first and last, there is only one bonded interaction
            if i==0 and self.n > 1:
                U_tot += self.compute_U_bond(sep=np.linalg.norm(self.coords[i]-self.coords[i+1]))
            elif i==self.n-1:
                U_tot += self.compute_U_bond(sep=np.linalg.norm(self.coords[i]-self.coords[i-1]))
            else:
                U_tot += self.compute_U_bond(sep=np.linalg.norm(self.coords[i]-self.coords[i-1])) 
                + self.compute_U_bond(sep=np.linalg.norm(self.coords[i]-self.coords[i+1]))
            # non-bonded
            dr = norm(self.coords[i]-self.coords, axis=-1)
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
    my_crw = CRW(100)
    my_crw.show()
    my_crw.write_sim_config("../../input/crw_100.txt")