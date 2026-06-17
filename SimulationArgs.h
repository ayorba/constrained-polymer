#ifndef SIMULATIONARGS
#define SIMULATIONARGS

#include<iostream>
#include <string>


// Read in command-line args


// simtype = {"NVE", "dampedMD", "collapse_polymer"}
// dt = time step for MD
// damping = damping coeffient for MD
// initial_temp = starting temperature of simulation
// writestep = Number of simulation steps between output of files
// OUT = path to where files will be output
// IN = path to where any input files are stored
// infile = the init_config file name to begin a simulation
// CF_mag = magnitude of the central force
// cont_sim = 1 to load an init_config file--this is the only option at the moment.


struct SimulationArgs
{
	std::string simtype;
	double dt;
	double damping;
	double initial_temp;
	int writestep;
	std::string IN;
	std::string infile;
	std::string OUT;
	double CF_mag;
	bool cont_sim;
};


void printUsage() 
{
   std::cout << "Usage: ./polymer <simtype> <dt> <damping> <initial_temp> <writestep> <IN> <infile> <OUT> <CF_mag> <cont_sim>\n"
             << "Example: ./polymer NVE 0.01 0.01 1e-5 1000000 /Desktop/config_files/ init_config.txt /Desktop/config_files/output/ 1e-4 0\n";
}


SimulationArgs parseCommandLine(int argc, char* argv[]) {
	if (argc != 11) 
	{
	   printUsage();
	   exit(EXIT_FAILURE);
	}

	SimulationArgs args;

	args.simtype = argv[1];
	args.dt = std::stod(argv[2]);
	args.damping = std::stod(argv[3]);
	args.initial_temp = std::stod(argv[4]);
	args.writestep = std::stoi(argv[5]);
	args.IN = argv[6];
	args.infile = argv[7];
	args.OUT = argv[8];
	args.CF_mag = std::stod(argv[9]);
	args.cont_sim = std::stoi(argv[10]);

	return args;

}


#endif


