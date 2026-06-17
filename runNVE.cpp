#include <iostream>
#include <iomanip>
#include "MD.h"


// Run NVE_MD()


void MD::runNVE(int numsteps, bool write)
{

	bool status_initialized = false;

	for(int step = 1; step < 1 + numsteps; ++step)
	{
		// Run MD step
		NVE_MD();

		// Check if Verlet should be remade
		makeVerletList();

		// Write Files
		if(step % writestep==0)
		{
			// Compute extra quantities
			computeTemp();
			sim->computeRg();
			interman->Etot = interman->KE + interman->PE;
			adjust_dt();

			if (status_initialized)
				std::cout << "\033[1A";  // overwrite previous two-line display

			std::cout << "\r[NVE] Step: " << std::setw(10) << step << " / " << numsteps
			          << "  |  Temp: " << std::scientific << std::setprecision(4) << temperature
			          << "  |  PE: "   << std::setprecision(6) << interman->PE
			          << "  |  dt: "   << std::setprecision(3) << dt
			          << "   \n"
			          << "[p] pause simulation   " << std::flush;
			status_initialized = true;

			// Write files for NVE steps only
			if(write)
			{
				writeFiles(step);
			}

			if (checkForPause(step))
				status_initialized = false;
		}
	}

	std::cout << "\n";

}


