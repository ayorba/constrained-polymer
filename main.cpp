#include <memory>
#include <math.h>
#include <unordered_set>
#include <utility> // std::pair
#include <functional> // std::hash
#include <iostream>
#include <string>
#include <algorithm>
#include "Sphere.h"
#include "Residue.h"
#include "Simulation.h"
#include "InteractionManager.h"
#include "PairHash.h"
#include "MD.h"
#include "SimulationArgs.h"

int main(int argc, char *argv[])
{

	SimulationArgs args = parseCommandLine(argc, argv);

	// MD parameters
	std::string simtype=args.simtype;
	double dt = args.dt;
	double damping = args.damping;
	double initial_temp = args.initial_temp;
	int writestep = args.writestep;
	std::string OUT = args.OUT;
	std::string IN = args.IN;
	std::string infile = args.infile;
	double CF_mag = args.CF_mag;
	bool cont_sim = args.cont_sim;


	// Set up objects
	Simulation sim;

	InteractionManager interman(&sim, CF_mag);

	// Set up a MD object
	MD md(&sim, &interman, IN, infile, OUT, dt, damping, writestep, initial_temp, simtype, CF_mag);

	// Load the sphere configuration, including bonds, bond angles, bond dihedrals
	md.loadConfig();

	// Set the number of atoms and residues
	sim.Nres = sim.residues.size();
	sim.Natoms = sim.spheres.size();

	std::cout<<"Nres: "<<sim.Nres<<"\n";
	std::cout<<"Natoms: "<<sim.Natoms<<"\n";


	// Make collection of all possible bonds that are not already made
	sim.makeNonPermanentBondPairs();

	std::cout<<"# Backbone bonds: "<<sim.backbone_pairs.size()<<"\n";
	std::cout<<"# Bond Angle constraints: "<<sim.bond_angles.size()<<"\n";
	std::cout<<"# Dihedral Angle constraints: "<<sim.bond_dihedrals.size()<<"\n";


	// Find the min/max diameter
	sim.findMinMaxDiam();

	// Update MD and InteractionManager with finalized Simulation object
	interman.sim = &sim;
	md.sim = &sim;
	md.interman = &interman;

	// Initialize system
	if(cont_sim==0)
	{
		md.initializeTemp();
	}

	md.makeVerletList(1);
	md.computeTemp();
	sim.computeRg();
	interman.computeInteractions();


	// Set the initial force_old for each sphere
	for(auto& sph : sim.spheres)
	{
		sph->force_old = sph->force - sph->mass*damping*sph->velocity;
	}

	interman.Etot = interman.KE + interman.PE;


	// Run one of the simtypes
	std::transform(simtype.begin(), simtype.end(), simtype.begin(), ::tolower);

	if(simtype=="nve")
	{
		md.writeFiles(0, 0, 1);

		int numsteps = 100000;
		md.runNVE(numsteps);
	}
	else if(simtype=="dampedmd")
	{
		md.writeFiles(0, 0, 1);

		md.runDampedMD();
	}
	else if(simtype=="collapse_polymer")
	{
		double CF_mag_for_collapse = interman.central_force_mag;

		if(cont_sim==0)
		{
			// Run NVE to get new initial configuration
			interman.central_force_mag = 0.0;
			int numsteps = 5000000;
			md.runNVE(numsteps, 0);
			
			// Write initial config to file
			md.writeFiles(0, 0, 1);

			// Run damped MD with central force to collapse
			interman.central_force_mag = CF_mag_for_collapse;
		}

		md.runDampedMD();
	}


	return 0;

}


