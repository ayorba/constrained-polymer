#include "Vector3D.h"
#include "Simulation.h"

Vector3D Simulation::getCOM()
{

	Vector3D COM;

	for(const auto& sph_ptr : spheres)
	{	
		auto* sph = sph_ptr.get();
		COM += sph->position;
	}

	COM /= Natoms;	

	return COM;
}


