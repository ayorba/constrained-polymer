#include <cmath>
#include <numeric>
#include "MD.h"
#include "Vector3D.h"


// This is used to keep track of nonbonded_pairs in Simulation.
// All other bonds are permanent, e.g., backbone_pairs, bond_angles, bond_dihedrals.
// Parameter bool initialize is used to make the initial Verlet list and bypass the displacement check


void MD::makeVerletList(bool initialize)
{

	// Check the displacements of all spheres
	// If the sum of the two max displacements is greater than the verlet_skin, 
	// remake the Verlet list
	

	double max_disp=0.0, max_disp_2=0.0;
	double disp_mag;
	for(const auto& sph : sim->spheres)
	{
		const auto& displacement = sph->displacement;
		disp_mag = displacement.norm();
		disp_mag > max_disp_2 ? disp_mag > max_disp ? max_disp = disp_mag : max_disp_2 = disp_mag : 0;
	}


	if(max_disp + max_disp_2 >= verlet_skin - 1.0 || initialize)
	{
		// Clear current list nonbonded_pairs
		sim->nonbonded_pairs.clear();

		// Zero the displacement vectors
		for(const auto& sph : sim->spheres)
		{
			sph->displacement.zeroVector();
		}


		// Remake nonbonded_pairs in Simulation.h
		// Iterate through all pairs of spheres that are not permanently bonded
		// These are all the bonds that may be temporarily interacting

		double dist;

		for(int bond_ind=0; bond_ind<sim->num_non_perm_bond_pairs; ++bond_ind)
		{
			const auto& bond = sim->all_non_perm_bond_pairs[bond_ind];

			// Fetch the Sphere objects from the Bond sphereMap
			auto it0 = bond->sphereMap.find(bond->sphereIDs[0]);
			auto it1 = bond->sphereMap.find(bond->sphereIDs[1]);
			if(it0 != bond->sphereMap.end() && it1 != bond->sphereMap.end())
			{
				auto* spha = it0->second.get();
				auto* sphb = it1->second.get();

				// Get distance between spheres
				dist = spha->distij(sphb, 0);
			
				// bond length for two particles that are not bonded is the sum of their radii
				if(dist/bond->bond_length <= verlet_skin)
				{
					sim->nonbonded_pairs.push_back(bond_ind);
				}
			}


		}		

	}

}


