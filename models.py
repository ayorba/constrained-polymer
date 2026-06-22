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
    # class-level attributes: sphere diameter, spring constants
    sigma = 1 # Backbone bead diameter
    U_bb  = 1 # Bond length potential spring constant
    e_rep = 1 # Repulsive steric potential spring constant

    rng = np.random.default_rng()

    def __init__(self, n: int):
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
            count=0
            # current position is coords[i-1]+dr, while U_rep bewteen coords[i] and all previous beads (not including the one it's bonded to) 
            while(self.compute_U_rep(np.sqrt(np.sum((self.coords[i-1]+dr - self.coords[:i-1])**2, axis=-1))) > 0):
                dr = self.rng.standard_normal(size=(3,))
                dr = dr / np.linalg.norm(dr)
            self.coords[i]=self.coords[i-1]+dr
            # res = scipy.optimize.minimize(fun=lambda r_i: self.compute_U_bond(sep=np.sqrt(np.sum((r_i-self.coords[i-1])**2))) + self.compute_U_rep(np.sqrt(np.sum((r_i - self.coords[:i-1])**2, axis=-1))), x0=self.coords[i-1]+dr)
            # print(f"({res.x[0]}, {res.x[1]}, {res.x[2]})")
            # self.coords[i]=res.x
    
    def show(self):
        print(f"CRW of length {self.n}:\n\tSigma: {self.sigma}\n\tU_bb: {self.U_bb}\n\te_rep: {self.e_rep}")
        print("Stats:")
        print(f"\tRadius of Gyration: {self.compute_rg()}")
        print(f"\tTotal Potential Energy: {self.compute_U_tot()}")
        print("Residues:")
        for i in range(self.n): 
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
            dr = norm(self.coords[i]-self.coords)
            U_tot += self.compute_U_rep(dr)

        return U_tot / 2

def norm(x: np.ndarray):
    return np.linalg.norm(x)

if __name__=="__main__":
    my_crw = CRW(100)
    my_crw.show()