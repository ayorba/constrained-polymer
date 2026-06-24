#include "InteractionManager.h"
#include "Vector3D.h"
#include <iostream>

// Include any external forces, e.g., central force, on each sphere

void InteractionManager::computeExternalInteractions()
{

	// Include central force on all spheres
	// The central force acts differently depending on the size of the sphere
	if(central_force_mag > 0.0)
	{
		double scale;
		//Vector3D unitba;
		Vector3D centralF;

		for(auto& sph_ptr : sim->spheres)
		{	
			auto* sph = sph_ptr.get();


			Vector3D r = -sph->position;
			double mag = r.norm();

			Vector3D unitba(0,0,0);

			if (mag > 1e-12)
				unitba = r * (1.0 / mag);
/*
			// unit vector from particle to origin (assumed to be approximate COM)
			//sph->position.print();
			unitba = -sph->position.normalized();
*/

			scale = central_force_mag * pow(sph->diameter / sim->max_diameter, 2.25);
			centralF = unitba*scale;

			sph->force += centralF;	

		}
	}
}

void InteractionManager::computeContinuousExternalInteractions() {
	if (central_force_mag > 0.0) {
		// force on each particle diminishes near origin
		double scale;
		Vector3D centralF;
		double fMag;
		double rMag;

		for (auto& sph_ptr : sim->spheres) {
			auto* sph = sph_ptr.get();

			Vector3D r = -sph->position;
			rMag = r.norm();

			Vector3D unitba(0,0,0);
			unitba = r * (1 / rMag);
			

			fMag = (rMag < sph->diameter/2) ? central_force_mag * rMag / (sph->diameter/2): central_force_mag;

			scale = fMag * pow(sph->diameter / sim->max_diameter, 2.25);
			centralF = unitba*scale;

			sph->force += centralF;

		}
	}
}




