There are six coarse-grained models for the proteins. Each has the same backbone, which is composed of beads of diameter $\sigma_{bb}\approx3.8 \mathring{A}$. Every model shares this same backbone, but may impose different potentials, or treat side chains differently. They are listed in order of increasing complexity. The potentials used are listed below:

1. the pairwise bond-length potential, $U_{\text{bond}}(r_{ij})$

$$U_{\text{bond}}(r_{ij}) = \frac{U_{\text{bb}}}{2}\left(1-\frac{r_{ij}}{\sigma_{ij}}\right)^2$$

$U_{\text{bb}}$ is set to be 

2. the pairwise nonbonded-interaction potential, $U_{\text{rep}}(r_{ij})$

$$U_{\text{rep}}(r_{ij}) = \frac{\epsilon_{\text{rep}}}{2}\left(1-\frac{r_{ij}}{\sigma_{ij}}\right)^2\Theta\left(1-\frac{r_{ij}}{\sigma_{ij}}\right)$$

3. the bond angle potential, $U_{\text{bend}}(\theta_{ijk})$

$$U_{\text{bend}}(\theta_{ijk}) = \frac{U_{\text{ba}}}{2}\left(1-\frac{\theta_{ijk}}{\theta_{ijk}^0}\right)^2$$

4. the dihedral angle potential, $U_{\text{dh}}(\psi_{ijkl})$

$$U_{\text{dh}}(\psi_{ijkl}) = U_{\text{da}}\sum_{\langle ijkl \rangle}\sum_{s=1}^4\left[A_s\cos(s\text{ }\psi_{ijkl})+B_s\sin(s\text{ }\psi_{ijkl})\right]$$

Notes:

- for $\sigma_{ij}$, note $\sigma_{ij} = \left(\sigma_i + \sigma_j\right)/2$ for two neighboring backbone beads
- $\epsilon_{\text{rep}}$ is the strength of the non-bonded repulsive interaction
- $\theta_{ijk}$ is between three consecutive backbone beads
- $\theta_{ijk}^0$ determined from the x-ray structure data
- $\psi_{ijkl}$ is between four consecutive backbone beads
- Fourier coefficients for $U_{\text{dh}}$ are calculated through statistical analysis

### CRW

Excluded-volume collapsed random walk

The simplest representation is a collapsed freely-jointed excluded-volume random-walk (CRW) model.

The only beads present are the backbone beads. The degrees of freedom are bond angles ($\theta_{ijk}$) and distance from one bead to the next ($r_{ij}$).

They are subject to the $U_{\text{bond}}$ bond length potential and the $U_{\text{rep}}$ steric repulsive potential.

### BADA

Bond angle and dihedral angle potentials

This model is the same as CRW, except the bond angle and dihedral potentials are also imposed.

The only beads present are the backbone beads. The degrees of freedom are bond angles ($\theta_{ijk}$), distance from one bead to the next ($r_{ij}$).

The polymer is subject to all potentials: $U_{\text{bond}}(r_{ij})$, $U_{\text{rep}}(r_{ij})$, $U_{\text{bend}}(\theta_{ijk})$, and  $U_{\text{dh}}(\psi_{ijkl})$

### FJSC

Freely-jointed side chain

This model is the exact same, except that now an additional side chain bead of diameter $\sigma_{sc}^i$ is added to every backbone bead. $\sigma_{sc}^i$ is chosen by randomly sampling from the distribution of ALL side chain diameters which has been determined through observation of the x-ray crystal structure data.

This new side chain bead is subject to the bond potential $U_{\text{bond}}(r_{ij})$ between itself and the backbone bead it is connected to, and also the repulsive non-bonded potential $U_{\text{rep}}$ between itself and any other bead.

Remember that when calculating $U_\text{bond}$, $\sigma_{ij}$ is the average of both diameters (sum of radii).

### In-Seq

In sequence side chain

A side chain bead of diameter $\sigma_{sc}^i$ is added to every backbone bead. $\sigma_{sc}^i$ is chosen by randomly sampling from the distribution of THIS SPECIFIC AMINO ACID's side chain diameters based on the x-ray crystal structure data.

This new side chain bead is subject to the bond potential $U_{\text{bond}}(r_{ij})$ between itself and the backbone bead it is connected to, and also the repulsive non-bonded potential $U_{\text{rep}}$ between itself and any other bead.

### MPSC

multi particle side chain

side chain characteristics are taken from the [martini](https://www.nature.com/articles/s41592-021-01098-3) model, glycine does not get a side chain bead. supplementary information [here](sftp://patrick@macpro:22/home/patrick/Projects/Constrained-Polymer-Collapse/papers/martini_supplementary_information.pdf)

### mod-MPSC

modified multi particle side chain - leucine and valine get two side chain beads