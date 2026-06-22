from Bio.PDB.PDBParser import PDBParser
import json
import requests
import numpy as np
import math
import matplotlib.pyplot as plt
import pandas as pd
import fetch_pdb
import os
import ipdb

class Vec3D:
    dim: int = 3
        
    def __init__(self, x: float, y: float, z: float):
        self.data = np.array((3))
        self.data[0]=x
        self.data[1]=y
        self.data[2]=z

        self.update_theta_phi: bool = True
        self.theta: float = 0
        self.phi: float = 0

        self.update_mag: bool = True
        self.magnitude: float = 0
        self.sq_magnitude: float = 0


    def vec(self)->np.ndarry:
        return self.data

    def mag(self)->float:
        if self.update_mag:
            self.sq_magnitude = np.sum(self.data**2, axis=0)
            self.magnitude = np.sqrt(self.sq_magnitude)
        return self.magnitude
    
    def theta(self)->float:
        pass

    def phi(self)->float:
        pass
    
    def x(self)->float:
        return self.data[0]
    
    def y(self)->float:
        return self.data[1]

    def z(self)->float:
        return self.data[2]


def load_pdb_file():
    pass

if __name__=="__main__":
    pdb_id = "5tkw"
    fetch_pdb.download_one(pdb_id=pdb_id, fmt="pdb", outdir="./")
    ipdb.set_trace()