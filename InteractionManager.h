#ifndef INTERACTION_MANAGER_H
#define INTERACTION_MANAGER_H


#include "Simulation.h"


class InteractionManager
{
	public:

		Simulation* sim;
		double central_force_mag;
	
		double KE;
		double PE;
		double Etot;

	
		double Fmag_total_avg;
		double Fmag_max;
		double Fmag_threshold;
		static constexpr double Fthreshold = 1e-13;//1e-14;

		double max_overlap;

		// Constructor
		InteractionManager(Simulation* simulation, double CF_mag=0.0) : sim{simulation}, central_force_mag{CF_mag}, KE{0.0}, PE{0.0} {}

		// Compute forces for all interactions
		// This just calls functions to compute all interactions below
		void computeInteractions();

		// Compute pairwise temporary interactions 
		void computeNonBondedInteractions();

		// Compute bonded interactions
		void computeBondInteractions();
		
		// Compute interactions from bond angles
		void computeAngleInteractions();
		//double computeAngleForces(Sphere* parti, Sphere* partj, Sphere* partk, Vector3D& force_i, Vector3D& force_j, Vector3D& force_k, BondAngle& bond);

		// Compute interactions from dihedral angles
		void computeDihedralInteractions();

		// Include central force on each sphere
		void computeExternalInteractions();

		// Zero the net force and potential energy on each sphere
		void zeroForceAndEnergy()
		{
			this->PE = 0.0;

			for(const auto& sph_ptr : sim->spheres)
			{
				auto* sph = sph_ptr.get();
				sph->force.zeroVector();
			}
		}

};


#endif

