#include <cmath>
#include <random>
#include "MD.h"
#include "Vector3D.h"


// Run molecular dynamics NVT algorithm

// Implements a Langevin MD NVT ensemble as described in "Computer Simulation of Liquids" (Allen, Tildesley) Sec. 12.2
// Attempts to equilibrate the temperature to target_temp
// damping is controlled by MD::damping


void MD::NVT_MD(double desired_temp)
{


	// Run Langevin MD
	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<double> noise(0.0, 1.0);

	const double exp_gamma_dt = exp(-damping*dt);	
	const double fluc_coeff = sqrt(1.0 - exp_gamma_dt*exp_gamma_dt);
	double fluct;
	Vector3D noise_vec;
	Vector3D disp;

	// Compute the velocity and position at t + dt/2
	for(const auto& sph_ptr : sim->spheres)
	{
		auto* sph = sph_ptr.get();

		sph->velocity += 0.5*dt * sph->force / sph->mass;
		disp = 0.5*dt * sph->velocity;
		sph->position += disp;
		sph->displacement += disp;
	}

	// Update frictional and stochastic terms
	for(const auto& sph_ptr : sim->spheres)
	{
		auto* sph = sph_ptr.get();
		noise_vec.x = noise(gen);
		noise_vec.y = noise(gen);
		noise_vec.z = noise(gen);

		fluct = fluc_coeff * sqrt(desired_temp/sph->mass);

		sph->velocity = exp_gamma_dt*sph->velocity + fluct * noise_vec;
	}

	// Compute the position at t + dt
	for(const auto& sph_ptr : sim->spheres)
	{
		auto* sph = sph_ptr.get();

		disp = 0.5*dt * sph->velocity;
		sph->position += disp;
		sph->displacement += disp;
	}

	// Compute the force at t + dt
	interman->computeInteractions();

	// Compute the velocity at t + dt/2
	for(const auto& sph_ptr : sim->spheres)
	{
		auto* sph = sph_ptr.get();
		sph->velocity += 0.5*dt * sph->force / sph->mass;
	}


}


