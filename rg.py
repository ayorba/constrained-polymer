import sys
import numpy as np
from Bio.PDB import PDBParser

def rg(pdb_file):
    parser = PDBParser(QUIET=True)
    struct = parser.get_structure("p", pdb_file)
    coords = np.array([r["CA"].get_vector().get_array()
                       for r in struct.get_residues() if "CA" in r])
    r_com = coords.mean(axis=0)
    return np.sqrt(((coords - r_com) ** 2).sum(axis=1).mean())

def rg_n(coords, n):
    """Average Rg over all subchains of length n."""
    windows = np.lib.stride_tricks.sliding_window_view(coords, (n, 3)).reshape(-1, n, 3)
    r_com = windows.mean(axis=1, keepdims=True)
    return np.sqrt(((windows - r_com) ** 2).sum(axis=2).mean(axis=1)).mean()

if __name__ == "__main__":
    import matplotlib
    matplotlib.rcParams.update({"mathtext.fontset": "stix", "font.family": "STIXGeneral"})
    import matplotlib.pyplot as plt
    import os
    pdb_file = sys.argv[1]
    parser = PDBParser(QUIET=True)
    struct = parser.get_structure("p", pdb_file)
    coords = np.array([r["CA"].get_vector().get_array()
                       for r in struct.get_residues() if "CA" in r])
    N = len(coords)
    ns = np.arange(1, N + 1)
    rg_vals = [rg_n(coords, n) for n in ns]
    plt.plot(ns, rg_vals)
    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel(r"$n$")
    plt.ylabel(r"$\langle R_g(n) \rangle\;(\mathring{\mathrm{A}})$")
    plt.title(os.path.basename(pdb_file))
    out = os.path.splitext(pdb_file)[0] + "_Rg_n.png"
    plt.savefig(out, dpi=150, bbox_inches="tight")
    print(f"saved {out}")
