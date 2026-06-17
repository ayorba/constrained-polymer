#include <cmath>
#include "Vector3D.h"
#include "Simulation.h"


void Simulation::computeRg()
{

	// Get COM
	Vector3D COM = getCOM();

	double Rg_ = 0.0;

	for(const auto& sph : spheres)
	{
		// Radius of gyration
		Rg_ += sph->distij(COM, 1);
	}
	Rg_ /= Natoms;
	Rg = sqrt(Rg_);
}


