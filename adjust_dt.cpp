#include <cmath>
#include <numeric>
#include <vector>
#include "MD.h"


// Get the average power in the system
// If the power is < 0, decrease the time step deltat
// if the power is > 0, increase teh time step deltat


void MD::adjust_dt()
{

	double avg_power = 0.0;

	for(const auto& sph : sim->spheres)
	{
		avg_power += sph->force.dot(sph->velocity);
	}
	avg_power /= sim->Natoms;

	// Adjust dt based on avg_power
	int pow_thresh = 25; // where did this come from?
	double scale_dt = 0.9; // also where did this come from?
	int dt_inc = 0;
	int dt_dec = 0;

	if(avg_power < 0.0 && dt > mindt)
	{
		dt_dec++;
		if(dt_dec >= pow_thresh)
		{
			dt_dec = 0;
			dt_inc = 0;
			dt *= scale_dt;
		}
	}

	else if(avg_power > 0.0 && dt < maxdt)
	{
		dt_inc++;
		if(dt_inc >= pow_thresh)
		{
			dt_inc = 0;
			dt_dec = 0;
			dt /= scale_dt;
		}
	}

}


