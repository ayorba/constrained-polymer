#include "MD.h"

// Run NVE molecular dynamics algorithm
// velocity-verlet integration scheme with constant energy
void MD::NVE_MD()
{

	// Compute the velocity at t + dt/2 and position at t + dt
	for(const auto& sph_ptr : sim->spheres)
	{
		auto* sph = sph_ptr.get();

		sph->velocity += 0.5*dt * (sph->force / sph->mass);
		sph->position += dt*sph->velocity;
		sph->displacement += dt*sph->velocity;
	}

	// Compute the force at t + dt
	interman->computeInteractions();

	// Compute the velocities at t + dt
	for(const auto& sph_ptr : sim->spheres)
	{
		auto* sph = sph_ptr.get();
		sph->velocity += 0.5*dt * (sph->force / sph->mass);
	}
}


