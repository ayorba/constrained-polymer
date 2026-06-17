# Constrained Polymer Collapse

A coarse-grained molecular dynamics (MD) simulator for studying the collapse of polymer chains — in particular, intrinsically disordered proteins (IDPs). Given an initial configuration file that encodes bead positions, bond topology, bond-angle constraints, and dihedral constraints, the program can equilibrate a chain via NVE or damped MD, and collapse it toward its center of mass by applying a central force.

The included example configuration (`5tkw_config_file_InSeq.txt`) is derived from the crystal structure of PDB entry **5TKW**.

---

## Table of Contents

1. [Physical Model](#physical-model)
2. [Architecture](#architecture)
   - [Class Overview](#class-overview)
   - [Source File Reference](#source-file-reference)
3. [Building](#building)
4. [Usage](#usage)
   - [Command-line Arguments](#command-line-arguments)
   - [Simulation Modes](#simulation-modes)
   - [Example Commands](#example-commands)
5. [Configuration File Format](#configuration-file-format)
6. [Output Files](#output-files)
7. [Continuing a Simulation](#continuing-a-simulation)

---

## Physical Model

The chain is represented as a sequence of **residues**, each composed of one backbone bead and zero or more side-chain beads. All beads are spheres with an explicit diameter and mass. Four types of interactions are computed every step:

### Bonded (permanent) interactions

| Interaction | Potential | Notes |
|---|---|---|
| **Bond stretch** (intra- and inter-residue) | Harmonic: $V = \frac{1}{2}k\!\left(1 - \frac{r}{r_0}\right)^2$ | Applied to intra-residue backbone↔side-chain bonds and inter-residue backbone↔backbone bonds |
| **Bond angle** | Harmonic: $V = \frac{1}{2}k(\theta - \theta_0)^2$ | Restrains the angle formed by three consecutive backbone beads |
| **Dihedral** | Fourier series (4 terms): $V = \sum_n \left[ A_n\cos(n\phi) + B_n\sin(n\phi) \right]$ | Coefficients from O'Hern et al., *"Calibrated Langevin-Dynamics simulations of intrinsically disordered proteins"* |

### Non-bonded (temporary) interactions

| Interaction | Potential | Notes |
|---|---|---|
| **Steric repulsion** | $V = \frac{1}{2}k\!\left(1 - \frac{r}{\sigma}\right)^2$ only when $r < \sigma$ | Purely repulsive; $\sigma = (d_i + d_j)/2$ (sum of radii). Non-bonded pairs are managed with a Verlet neighbor list |

### External interactions

| Interaction | Description |
|---|---|
| **Central force** | An inward radial force toward the origin (approximate center of mass). Magnitude is scaled by $(d_i/d_{\max})^{2.25}$ so larger beads are pulled more strongly. Used only in `collapse_polymer` mode. |

---

## Architecture

### Class Overview

```
main.cpp
│
├── SimulationArgs       — Parses command-line arguments
│
├── Simulation           — Owns all simulation data
│   ├── vector<Sphere>         — All beads in the system
│   ├── vector<Residue>        — Residues, each with backbone + side-chain spheres
│   ├── vector<BondPair>       — Permanent inter-residue backbone bonds
│   ├── vector<BondAngle>      — Three-body angle constraints
│   ├── vector<BondDihedral>   — Four-body dihedral constraints
│   └── vector<BondPair>       — Non-permanent pairs (rebuilt via Verlet list)
│
├── InteractionManager   — Computes all forces and energies
│   ├── computeBondInteractions()        — Harmonic bonds (intra + inter residue)
│   ├── computeAngleInteractions()       — Harmonic bond angles
│   ├── computeDihedralInteractions()    — Fourier dihedral potential
│   ├── computeNonBondedInteractions()   — Repulsive steric contacts (Verlet list)
│   └── computeExternalInteractions()    — Central force (collapse mode)
│
└── MD                   — Integrators and I/O
    ├── loadConfig()        — Reads configuration file into Simulation
    ├── initializeTemp()    — Assigns random velocities at target temperature
    ├── makeVerletList()    — Rebuilds neighbor list when beads displace enough
    ├── NVE_MD()            — Single velocity-Verlet step (NVE)
    ├── runNVE()            — Runs NVE for a fixed number of steps
    ├── dampedMD()          — Single damped MD step (velocity-Verlet + friction)
    ├── runDampedMD()       — Runs damped MD until forces converge
    ├── adjust_dt()         — Adaptive time-step based on average power
    └── writeFiles()        — Writes all output files
```

### Integrators

**NVE (velocity-Verlet):** Standard symplectic integration conserving total energy.

$$
\begin{aligned}
v\!\left(t + \frac{dt}{2}\right) &= v(t) + \frac{dt}{2} \cdot \frac{F(t)}{m} \\
x(t + dt) &= x(t) + dt \cdot v\!\left(t + \frac{dt}{2}\right) \\
F(t + dt) &= \text{computeInteractions()} \\
v(t + dt) &= v\!\left(t + \frac{dt}{2}\right) + \frac{dt}{2} \cdot \frac{F(t + dt)}{m}
\end{aligned}
$$

**Damped MD (velocity-Verlet with friction):** Dissipates kinetic energy toward a local energy minimum. Used for structure relaxation and collapse.

$$
\begin{aligned}
x(t + dt) &= x(t) + v(t)\,dt + \frac{dt^2}{2} \cdot \frac{F_\text{old}}{m} \\
F(t + dt) &= \text{computeInteractions()} \\
F_\text{damped} &= \frac{F(t+dt) - m\gamma\, v(t) - \tfrac{dt\,\gamma}{2} F_\text{old}}{1 + \gamma\,dt/2} \\
v(t + dt) &= v(t) + \frac{dt}{2} \cdot \frac{F_\text{old} + F_\text{damped}}{m}
\end{aligned}
$$

The damped integrator terminates when the maximum force magnitude on any bead falls below `Fthreshold = 1e-13` (and at least 50,000 steps have been run).

**Adaptive time step (`adjust_dt`):** The average power $P = \sum_i F_i \cdot v_i / N$ is computed at each write step. If `P < 0` the system is moving uphill and `dt` is reduced by a factor of 0.9; if `P > 0` it is increased. Bounds: `mindt = 1e-5`, `maxdt = 1.0`.

**Verlet neighbor list:** Non-bonded pairs are tracked in `Simulation::nonbonded_pairs`. The list is rebuilt whenever the sum of the two largest bead displacements since the last rebuild exceeds `verlet_skin - 1.0` (where `verlet_skin = 1.2`). Permanently bonded pairs are excluded from this list entirely.

### Source File Reference

| File | Responsibility |
|---|---|
| `main.cpp` | Entry point; orchestrates setup and selects simulation mode |
| `MD.h` | `MD` class declaration and inline `computeKE`/`computeTemp` |
| `Simulation.h` | `Simulation` class with all data containers and topology helpers |
| `InteractionManager.h` | `InteractionManager` class declaration |
| `Sphere.h` | Single bead: position, velocity, force, diameter, mass |
| `Residue.h` | One residue: backbone bead, side-chain beads, intra-residue bonds |
| `BondPair.h` | Two-body bond: sphere IDs, stiffness, equilibrium length |
| `BondAngle.h` | Three-body angle constraint |
| `BondDihedral.h` | Four-body dihedral constraint |
| `Vector3D.h` | 3D vector math (dot, cross, norm, arithmetic operators) |
| `PairHash.h` | Hash functor for `unordered_set<pair<int,int>>` |
| `SimulationArgs.h` | Command-line parsing |
| `loadConfig.cpp` | Reads a config file into `Simulation` |
| `writeFiles.cpp` | Writes all output files |
| `NVE_MD.cpp` | Single NVE velocity-Verlet step |
| `runNVE.cpp` | NVE loop for a fixed number of steps |
| `dampedMD.cpp` | Single damped MD step |
| `runDampedMD.cpp` | Damped MD loop until convergence |
| `NVT_MD.cpp` / `runNVT.cpp` | NVT integrator (present but not wired into `main`) |
| `computeBondInteractions.cpp` | Harmonic bond forces |
| `computeAngleInteractions.cpp` | Harmonic angle forces |
| `computeDihedralInteractions.cpp` | Fourier dihedral forces |
| `computeNonBondedInteractions.cpp` | Repulsive steric forces (Verlet list) |
| `computeExternalInteractions.cpp` | Central collapse force |
| `computeInteractions.cpp` | Calls all force routines in sequence |
| `makeVerletList.cpp` | Rebuilds non-bonded neighbor list |
| `initializeTemp.cpp` | Assigns Maxwell-Boltzmann velocities |
| `getCOM.cpp` | Computes center of mass |
| `computeRg.cpp` | Computes radius of gyration |
| `adjust_dt.cpp` | Adaptive time-step logic |
| `Makefile` | Builds the `polymer` executable with `-std=c++17 -O3` |

---

## Building

Requires a C++17-compatible compiler (GCC 7+ or Clang 5+).

```bash
make
```

This compiles all `.cpp` files and links them into the `polymer` executable. Object files and dependency files are removed automatically after a successful build. To remove the executable as well:

```bash
make clean
```

---

## Usage

```
./polymer <simtype> <dt> <damping> <initial_temp> <writestep> <IN> <infile> <OUT> <CF_mag> <cont_sim>
```

### Command-line Arguments

| Argument | Type | Description |
|---|---|---|
| `simtype` | string | Simulation mode: `NVE`, `dampedMD`, or `collapse_polymer` (case-insensitive) |
| `dt` | float | MD time step |
| `damping` | float | Friction coefficient $\gamma$ for damped MD (unused in NVE) |
| `initial_temp` | float | Initial temperature for velocity initialization (in reduced units) |
| `writestep` | int | Number of MD steps between output file writes |
| `IN` | string | Path to the directory containing the input config file. **Must include a trailing slash.** |
| `infile` | string | Name of the input configuration file |
| `OUT` | string | Path to the output directory. **Must exist and include a trailing slash.** |
| `CF_mag` | float | Magnitude of the central collapse force. Set to `0` if not collapsing. |
| `cont_sim` | int | `0` = fresh start (initialize velocities from `initial_temp`); `1` = continue from existing config (velocities loaded from `infile`) |

### Simulation Modes

#### `NVE`

Constant-energy MD using the velocity-Verlet algorithm. Velocities are initialized from a Maxwell-Boltzmann distribution at `initial_temp`. Runs for a fixed **100,000 steps**. No central force is applied.

Best used for: short equilibration runs or testing that the configuration is reasonable.

#### `dampedMD`

Overdamped MD that dissipates kinetic energy until the maximum force on any bead drops below `Fthreshold = 1e-13`, with a minimum of 50,000 steps. The time step is adapted automatically via `adjust_dt`. No central force is applied unless `CF_mag > 0`.

Best used for: relaxing a starting structure to remove steric clashes.

#### `collapse_polymer`

Two-phase protocol for collapsing a chain:

1. **Equilibration phase** (skipped if `cont_sim=1`): Runs 5,000,000 NVE steps with `CF_mag = 0` to generate a thermally equilibrated starting conformation. The configuration at the end of this phase is written to `config_file.txt`.
2. **Collapse phase**: Runs damped MD with the central force (`CF_mag`) turned on until forces converge.

Best used for: generating compact, collapsed polymer conformations starting from an extended or native structure.

### Example Commands

**Collapse the included 5TKW protein config from scratch:**
```bash
mkdir -p output
./polymer collapse_polymer 0.01 0.1 1e-5 10000 ./input 5tkw_config_file_InSeq.txt ./output/ 1e-4 0
```

**Short NVE equilibration at temperature 0.5:**
```bash
./polymer NVE 0.005 0.0 0.5 1000 ./input 5tkw_config_file_InSeq.txt ./output/ 0.0 0
```

**Continue a collapse from a previously written config:**
```bash
./polymer collapse_polymer 0.01 0.1 1e-5 10000 ./output/ ./input/config_file.txt ./output/ 1e-4 1
```

**Relax a structure with damped MD only (no collapse force):**
```bash
./polymer dampedMD 0.01 0.5 1e-6 5000 ./input 5tkw_config_file_InSeq.txt ./output/ 0.0 0
```

---

## Configuration File Format

The input file is a comma-delimited text file. The same format is used for both input (via `loadConfig`) and output (via `writeFiles`), so any written snapshot can be fed back in as a new starting configuration.

Records appear in this order: all residue blocks first, then inter-residue bonds, then angle constraints, then dihedral constraints.

### Record Types

#### `ATOM` — bead definition

```
ATOM,<resID>,<atomID>,<x>,<y>,<z>,<diameter>,<mass>
```

When loading a previously written snapshot (which includes dynamics state):
```
ATOM,<resID>,<atomID>,<x>,<y>,<z>,<diameter>,<mass>,<vx>,<vy>,<vz>,<Fx>,<Fy>,<Fz>
```

Within each residue block, the **first** `ATOM` line is the backbone bead; all subsequent `ATOM` lines before the `INTRA_RESIDUE` line are side-chain beads.

#### `INTRA_RESIDUE` — bond within a residue

```
INTRA_RESIDUE,<atomID_a>,<atomID_b>,<stiffness>,<equil_length>
```

This record ends the residue block. For residues with no intra-residue bond (single-bead residues), write `INTRA_RESIDUE` with no fields.

#### `INTER_RESIDUE` — backbone bond between consecutive residues

```
INTER_RESIDUE,<atomID_a>,<atomID_b>,<stiffness>,<equil_length>
```

#### `BOND_ANGLE` — three-body angle constraint

```
BOND_ANGLE,<atomID_i>,<atomID_j>,<atomID_k>,<stiffness>,<fixed_angle>,<current_angle>
```

Angles are in radians. `fixed_angle` is the equilibrium angle; `current_angle` is updated at runtime.

#### `BOND_DIHEDRAL` — four-body dihedral constraint

```
BOND_DIHEDRAL,<atomID_i>,<atomID_j>,<atomID_k>,<atomID_l>,<stiffness>,<current_angle>
```

The dihedral potential uses the Fourier form from O'Hern et al. with fixed coefficients; only `stiffness` scales the overall amplitude.

#### `END`

Marks the end of the file.

### Example (two-bead residue)

```
ATOM,0,0,5.338,7.910,2.357,1.0,1.0
ATOM,0,1,4.381,8.767,2.960,1.837,1.0
INTRA_RESIDUE,0,1,1.0,1.418
ATOM,1,2,6.255,8.276,2.517,1.0,1.0
ATOM,1,3,6.217,9.858,2.085,2.281,1.0
INTRA_RESIDUE,2,3,1.0,1.640
INTER_RESIDUE,0,2,1.0,1.458
BOND_ANGLE,0,2,4,1.0,1.872,1.872
BOND_DIHEDRAL,0,2,4,6,1.0,0.314
END
```

---

## Output Files

All output files are written to the `OUT` directory specified on the command line.

| File | Written | Contents |
|---|---|---|
| `config_file.txt` | Every `writestep` steps (overwritten) | Full configuration snapshot in the input file format, including velocities and forces. Can be used directly as `infile` with `cont_sim=1`. |
| `final_config.xyzr` | End of simulation (overwritten) | Tab-separated table: `atomID  x  y  z  radius  residueID`. Convenient for analysis and packing-fraction calculations. |
| `traj.xyzr` | Every `writestep` steps (appended) | OVITO-compatible extended XYZ trajectory. Each frame is: atom count, `Timestep <N>`, then one `x y z radius` line per bead. Load directly in OVITO for visualization. |
| `temp_rg_Etot.txt` | Every `writestep` steps (appended) | Tab-separated time series with columns `temp`, `radius_of_gyration`, `Etot`, `Fmag_max`. Useful for monitoring convergence. |
| `xyzr_-1_povray.txt` | End of simulation | Bead positions and colors formatted for Blender/POV-Ray rendering. Backbone beads are colored blue; side-chain beads green. |

### Monitoring convergence during `dampedMD` / `collapse_polymer`

Watch `temp_rg_Etot.txt`. The simulation has converged when:
- `Fmag_max` is at or near `1e-13`
- `Etot` has plateaued
- `radius_of_gyration` is stable
