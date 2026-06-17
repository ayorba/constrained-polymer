#include <vector>
#include <cmath>
#include <numeric>
#include "MD.h"
#include "Vector3D.h"


// Run a damped molecular dynamics algorithm
// damping set by MD::damping


void MD::dampedMD()
{

	double deltat_sq = dt*dt;
	Vector3D disp;


	// Compute the position at t + dt
	for(const auto& sph_ptr : sim->spheres)
	{
		auto* sph = sph_ptr.get();
		disp = (sph->velocity*dt) + (0.5*deltat_sq * (sph->force_old / sph->mass));
		sph->position += disp;
		sph->displacement += disp;
	}


	// Compute the force at t + dt
	interman->computeInteractions();


	// Include damping in force
	for(const auto& sph_ptr : sim->spheres)
	{	
		auto* sph = sph_ptr.get();
		sph->force = (sph->force - (sph->mass*damping*sph->velocity) - ((0.5*dt*damping)*sph->force_old)) / (1.0 + 0.5*damping*dt);
	}


	// Check the force balance
	interman->Fmag_max = 0.0;
	double Fmag;
	for(const auto& sph_ptr : sim->spheres)
	{		
		auto* sph = sph_ptr.get();

		// Using the max Fmag as the metric to determine when the system is sufficiently damped
		Fmag = sph->force.norm();

		// Find the max Fmag on any sphere
		Fmag > interman->Fmag_max ? interman->Fmag_max=Fmag : 0;
	}


	// Compute the velocities at t + dt
	for(const auto& sph_ptr : sim->spheres)
	{	
		auto* sph = sph_ptr.get();
		sph->velocity += 0.5*(sph->force_old + sph->force) * (dt / sph->mass);
	}


	// update previous forces
	for(const auto& sph_ptr : sim->spheres)
	{	
		auto* sph = sph_ptr.get();
		sph->force_old = sph->force;
	}


}


