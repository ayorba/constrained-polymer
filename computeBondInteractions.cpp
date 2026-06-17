#include <memory>
#include <unordered_map>
#include "InteractionManager.h"
#include "Sphere.h"
#include "Residue.h"
#include "Bond.h"
#include "Vector3D.h"


// Compute force and energy between all permanently bonded pairs of spheres
// These all interact through a harmonic potential


void InteractionManager::computeBondInteractions()
{
	double equil_length;
	Vector3D unitba;
	double dist, Fmag, dist_ratio;


	// Intra-residue interactions

	for(const auto& res : sim->residues)
	{
		for(const auto& bond : res.bonds)
		{
			auto it0 = bond.sphereMap.find(bond.sphereIDs[0]);
			auto it1 = bond.sphereMap.find(bond.sphereIDs[1]);
			if(it0 != bond.sphereMap.end() && it1 != bond.sphereMap.end())
			{
				auto* spha = it0->second.get();
				auto* sphb = it1->second.get();


				equil_length = bond.bond_length;		
				unitba = spha->vecij(sphb, 1);
				dist = spha->distij(sphb);
				dist_ratio = dist / equil_length;
				Fmag = bond.stiffness * (1.0/equil_length) * (1.0 - dist_ratio);

				// Add forces to spheres
				spha->force -= Fmag*unitba;
				sphb->force += Fmag*unitba;


				// Add potential energy from interaction
				this->PE += 0.5*bond.stiffness*(1.0 - dist_ratio)*(1.0 - dist_ratio);

				// Check if this bond has max overlap in system
				if(1.0 - dist_ratio > max_overlap)
				{
					max_overlap = 1.0 - dist_ratio;
				}

			}
		}

	}


	// Inter-residue interactions
	for(const auto& bond : sim->backbone_pairs)
	{
		auto it0 = bond.sphereMap.find(bond.sphereIDs[0]);
		auto it1 = bond.sphereMap.find(bond.sphereIDs[1]);
		if(it0 != bond.sphereMap.end() && it1 != bond.sphereMap.end())
		{
			auto* spha = it0->second.get();
			auto* sphb = it1->second.get();


			equil_length = bond.bond_length;

			unitba = spha->vecij(sphb, 1);	
			dist = spha->distij(sphb);
			dist_ratio = dist / equil_length;

			Fmag = (1.0/equil_length)*(1.0 - dist_ratio);

			// Add forces to spheres
			spha->force -= Fmag*unitba;
			sphb->force += Fmag*unitba;

			// Add potential energy from interaction
			this->PE += 0.5*bond.stiffness*(1.0 - dist_ratio)*(1.0 - dist_ratio);

			// Check if this bond has max overlap in system
			if(1.0 - dist_ratio > max_overlap)
			{
				max_overlap = 1.0 - dist_ratio;
			}

		}
	}


}


