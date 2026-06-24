#include "InteractionManager.h"
#include "Sphere.h"
#include "Residue.h"
#include "Bond.h"


void InteractionManager::computeInteractions()
{
	// Reset max_overlap
	// Find bond that has the largest overlap in the system
	// using functions below
	max_overlap = 0.0;

	// Set net force on each particle and total PE to zero 
	zeroForceAndEnergy();

	// Compute all forces and total potential energy
	computeNonBondedInteractions();
	computeBondInteractions();
	computeAngleInteractions();
	computeDihedralInteractions();
	//computeExternalInteractions();
	computeContinuousExternalInteractions();

}


