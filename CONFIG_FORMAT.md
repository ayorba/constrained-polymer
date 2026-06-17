# Configuration File Format

Config files are read by `MD::loadConfig()` (`loadConfig.cpp`) and written by
`MD::writeFiles()` (`writeFiles.cpp`). The two functions are inverses of each
other, so any file written by the simulation can be loaded back in directly.

## General rules

- **Delimiter**: every field on a line is separated by a single comma (`,`).
- **Label**: the first field on each line is a keyword that determines how the
  rest of the line is parsed. Unknown labels (including `END`) are silently
  ignored.
- **No header line.**
- **No blank lines** (the parser does not skip them; a blank line produces an
  empty label which is silently ignored, so they are harmless but
  unconventional).
- Values are parsed with `std::stoi` / `std::stod`, so standard decimal and
  scientific notation (`1e-4`, `-3.14`) are accepted.

---

## Section order

The writer always emits sections in this order; the reader is label-driven and
does not strictly enforce it, but mixing the order can break residue grouping
(see [Residue grouping](#residue-grouping) below).

```
[ATOM / INTRA_RESIDUE blocks – one group per residue, in residue order]
[INTER_RESIDUE lines]
[BOND_ANGLE lines]
[BOND_DIHEDRAL lines]
END
```

---

## Line types

### `ATOM`

One line per sphere. Two forms are accepted:

**Full form** (position + velocity + force):
```
ATOM,<resID>,<atomID>,<x>,<y>,<z>,<diameter>,<mass>,<velx>,<vely>,<velz>,<Fx>,<Fy>,<Fz>
```

**Short form** (position only; velocity and force initialise to zero):
```
ATOM,<resID>,<atomID>,<x>,<y>,<z>,<diameter>,<mass>
```

| Field | Type | Description |
|-------|------|-------------|
| `resID` | int | Residue this sphere belongs to (0-indexed) |
| `atomID` | int | Unique sphere identifier (0-indexed) |
| `x`, `y`, `z` | double | Position |
| `diameter` | double | Sphere diameter (not radius) |
| `mass` | double | Sphere mass |
| `velx`, `vely`, `velz` | double | Velocity (full form only) |
| `Fx`, `Fy`, `Fz` | double | Force (full form only) |

The parser distinguishes the two forms by checking whether more than 7 value
fields are present after the label.

**Examples:**
```
ATOM,0,0,1.23,0.00,-0.45,1.0,1.0,0.001,-0.002,0.0,0.0,0.0,0.0
ATOM,0,1,2.50,0.00,-0.45,1.0,1.0
```

---

### `INTRA_RESIDUE`

Appears immediately after the `ATOM` lines for a residue. Two forms:

**Bond form** (one bond per line):
```
INTRA_RESIDUE,<atomID_a>,<atomID_b>,<stiffness>,<equil_length>
```

**Empty form** (residue has no intra-residue bonds):
```
INTRA_RESIDUE
```

| Field | Type | Description |
|-------|------|-------------|
| `atomID_a`, `atomID_b` | int | The two spheres forming the bond (`atomID_a < atomID_b`) |
| `stiffness` | double | Spring constant for the harmonic bond potential |
| `equil_length` | double | Equilibrium bond length |

Multiple `INTRA_RESIDUE` bond lines in a row are all assigned to the same
residue. The residue is finalised when the label changes (to `INTRA_RESIDUE`
of the next residue, or to any other label).

The empty form (`INTRA_RESIDUE` with no comma-separated fields) acts as a
residue terminator when there are no intra-residue bonds to record.

**Examples:**
```
INTRA_RESIDUE,1,2,500.0,0.38
INTRA_RESIDUE,1,3,500.0,0.38
INTRA_RESIDUE
```

---

### `INTER_RESIDUE`

One line per backbone (inter-residue) bond. All `INTER_RESIDUE` lines appear
after all residue blocks.

```
INTER_RESIDUE,<atomID_a>,<atomID_b>,<stiffness>,<equil_length>
```

| Field | Type | Description |
|-------|------|-------------|
| `atomID_a`, `atomID_b` | int | The two bonded spheres |
| `stiffness` | double | Spring constant |
| `equil_length` | double | Equilibrium bond length |

**Example:**
```
INTER_RESIDUE,0,4,500.0,0.38
```

---

### `BOND_ANGLE`

One line per bond-angle constraint, after all `INTER_RESIDUE` lines.

```
BOND_ANGLE,<atomID_a>,<atomID_b>,<atomID_c>,<stiffness>,<fixed_angle>,<current_angle>
```

| Field | Type | Description |
|-------|------|-------------|
| `atomID_a`, `atomID_b`, `atomID_c` | int | Three spheres defining the angle; `b` is the vertex |
| `stiffness` | double | Spring constant for the angle potential |
| `fixed_angle` | double | Equilibrium (target) angle in radians |
| `current_angle` | double | Angle at the time the file was written (radians) |

**Example:**
```
BOND_ANGLE,0,4,8,50.0,1.9199,1.9199
```

---

### `BOND_DIHEDRAL`

One line per dihedral-angle constraint, after all `BOND_ANGLE` lines.

```
BOND_DIHEDRAL,<atomID_a>,<atomID_b>,<atomID_c>,<atomID_d>,<stiffness>,<current_angle>
```

| Field | Type | Description |
|-------|------|-------------|
| `atomID_a`–`atomID_d` | int | Four spheres defining the dihedral |
| `stiffness` | double | Spring constant for the dihedral potential |
| `current_angle` | double | Dihedral angle at the time the file was written (radians) |

**Example:**
```
BOND_DIHEDRAL,0,4,8,12,10.0,-1.5708
```

---

### `END`

```
END
```

Written as the final line by `writeFiles`. The parser ignores it (no matching
label branch), so it is optional on input.

---

## Residue grouping

Within a config file, each residue is represented as a block:

```
ATOM,<resID>,<atomID_backbone>,...   ← first ATOM for this resID = backbone
ATOM,<resID>,<atomID_sc1>,...        ← subsequent ATOMs = side-chain spheres
...
INTRA_RESIDUE,...                    ← bonds within the residue (zero or more)
INTRA_RESIDUE                        ← empty terminator if no bonds
```

**Backbone vs side-chain**: the *first* `ATOM` line for a given `resID` is
treated as the backbone sphere; all subsequent `ATOM` lines with the same
`resID` are side-chain spheres.

**Residue boundary detection** uses two mechanisms:

1. **With `INTRA_RESIDUE`**: the residue is finalised when the parser
   encounters any label other than `INTRA_RESIDUE` after one or more
   `INTRA_RESIDUE` lines.

2. **Without `INTRA_RESIDUE`** (e.g. single-atom residues): the residue is
   finalised when the `resID` changes inside the `ATOM` block, or when the
   label transitions away from `ATOM` to something other than `INTRA_RESIDUE`.

---

## Complete minimal example

A two-residue, two-atom polymer with one backbone bond and a bond-angle
constraint; no side chains, no dihedral angles:

```
ATOM,0,0,0.0,0.0,0.0,1.0,1.0
INTRA_RESIDUE
ATOM,1,1,1.0,0.0,0.0,1.0,1.0
ATOM,1,2,1.5,0.5,0.0,0.8,0.8
INTRA_RESIDUE,1,2,500.0,0.65
INTER_RESIDUE,0,1,500.0,1.0
BOND_ANGLE,0,1,2,50.0,1.9199,1.9199
END
```

Breakdown:
- Residue 0: one backbone sphere (atomID 0) at the origin, no bonds.
- Residue 1: backbone sphere (atomID 1) plus one side-chain sphere (atomID 2),
  with one intra-residue bond between them.
- One backbone bond connecting atomID 0 and atomID 1.
- One bond-angle constraint on the triplet (0, 1, 2).

---

## Atom ID fast-path assumption

When looking up spheres by atomID for bond/angle/dihedral records, the parser
first tries `loadedspheres[atomID]` directly (O(1)). This fast path is valid
when atomIDs are **0-indexed and appear in strictly ascending order** in the
file, which is always true for files produced by `writeFiles`. If atomIDs are
not contiguous or not 0-indexed the parser falls back to a linear scan, so
arbitrary atomID schemes still work, just slower to load.
