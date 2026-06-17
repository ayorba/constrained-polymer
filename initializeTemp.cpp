#include <random>
#include "MD.h"
#include "Vector3D.h"


// Set initial temperature
// zero the average linear momentum


void MD::initializeTemp()
{

	Vector3D lin_mom;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<double> gaussian_vel(0.0, 1.0);
	double desired_temp = temperature;


	// Set random velocities
	for(const auto& sph_ptr : sim->spheres)
	{
		auto* sph = sph_ptr.get();
		sph->setVelocity(gaussian_vel(gen), gaussian_vel(gen), gaussian_vel(gen));
	}


	// Shift the velocities so the total linear momentum is zero

	// Compute the average total linear momentum        
	for(const auto& sph_ptr : sim->spheres)
	{
		auto* sph = sph_ptr.get();
		lin_mom += sph->mass*sph->velocity;
	}

	lin_mom /= sim->Natoms;


	// Remove the average linear momentum from velocities
	for(const auto& sph_ptr : sim->spheres)
	{
		auto* sph = sph_ptr.get();
		sph->velocity -= lin_mom/sph->mass;
	}


	// Scale the initial velocities to have the correct temperature
	computeTemp();

	double scale = sqrt(desired_temp / temperature);
	for(const auto& sph_ptr : sim->spheres)
	{
		auto* sph = sph_ptr.get();
		sph->velocity *= scale;
	}


	// Set the current temperature and KE
	computeTemp();


}


