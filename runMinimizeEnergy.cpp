#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <numeric>
#include "MD.h"


// Run dampedMD()
// Uses InteractionManager::force_threshold and InteractionManager::central_force_mag


void MD::runMinimizeEnergy()
{
	bool status_initialized = false;
	int step = 1;

	do
	{
		// Run MD step
		minimizeEnergy();

		// Check to see if Verlet list should be remade
		makeVerletList();

		// Write Files
		if(step % writestep==0) // Compute extra quantities
		{
			computeTemp();
			sim->computeRg();
			interman->Etot = interman->KE + interman->PE;
			adjust_dt();

			if (status_initialized)
				std::cout << "\033[1A";  // overwrite previous two-line display

			std::cout << "\r[MinimizeEnergy] Step: " << std::setw(10) << step
			          << "  |  Temp: " << std::scientific << std::setprecision(4) << temperature
			          << "  |  PE: "   << std::setprecision(6) << interman->PE
			          << "  |  Fmax: " << std::setprecision(3) << interman->Fmag_max
			          << "  |  dt: "   << dt
			          << "   \n"
			          << "[p] pause simulation   " << std::flush;
			status_initialized = true;

			writeFiles(step);

			if (checkForPause(step))
				status_initialized = false;

		}

		// increment step
		step++;
	}
	// when relaxing to remove overlaps:
	while(interman->Fmag_max > interman->Fthreshold || step < 50000);


	// Write final configuration to file
	computeTemp();
	sim->computeRg();
	interman->Etot = interman->KE + interman->PE;

	std::cout << "\n";

	writeFiles(step+1, 1, 1);

}


