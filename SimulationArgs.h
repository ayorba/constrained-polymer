#ifndef SIMULATIONARGS
#define SIMULATIONARGS

#include <iostream>
#include <string>
#include <unordered_map>


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

void printUsage2() 
{
   std::cout << "Usage: ./polymer <simtype> <dt> <damping> <initial_temp> <writestep> <IN> <infile> <OUT> <CF_mag> <cont_sim>\n"
             << "Example: ./polymer NVE 0.01 0.01 1e-5 1000000 /Desktop/config_files/ init_config.txt /Desktop/config_files/output/ 1e-4 0\n";
}

void printUsage()
{
	const char* program = "polymer";
    std::cout
        << "Usage:\n"
        << "  " << program
        << " <simtype> <dt> <damping> <initial_temp>"
        << " <writestep> <IN> <infile> <OUT> <CF_mag> <cont_sim>\n\n"

        << "Labeled form:\n"
        << "  " << program << "\n"
        << "    --simtype TYPE\n"
        << "    --dt VALUE\n"
        << "    --damping VALUE\n"
        << "    --initial-temp VALUE\n"
        << "    --writestep INTEGER\n"
        << "    --in DIRECTORY\n"
        << "    --infile FILE\n"
        << "    --out DIRECTORY\n"
        << "    --cf-mag VALUE\n"
        << "    --cont-sim 0|1\n\n"

        << "Options:\n"
        << "  --simtype TYPE        Simulation type, for example NVE\n"
        << "  --dt VALUE            Simulation time step; must be > 0\n"
        << "  --damping VALUE       Damping value; must be >= 0\n"
        << "  --initial-temp VALUE  Initial temperature; must be >= 0\n"
        << "  --writestep INTEGER   Output interval; must be > 0\n"
        << "  --in DIRECTORY        Input directory\n"
        << "  --infile FILE         Input configuration filename\n"
        << "  --out DIRECTORY       Output directory\n"
        << "  --cf-mag VALUE        CF magnitude; must be >= 0\n"
        << "  --cont-sim 0|1        Continue an existing simulation\n"
        << "  -h, --help            Show this help message\n";
}

void printArgs(SimulationArgs* args)
{
	std::cout<<"SimType: "<<args->simtype<<"\n";
	std::cout<<"dt: "<<args->dt<<"\n";
	std::cout<<"damping: "<<args->damping<<"\n";
	std::cout<<"initial_temp: "<<args->initial_temp<<"\n";
	std::cout<<"writestep: "<<args->writestep<<"\n";
	std::cout<<"IN: "<<args->IN<<"\n";
	std::cout<<"infile: "<<args->infile<<"\n";
	std::cout<<"OUT: "<<args->OUT<<"\n";
	std::cout<<"CF_mag: "<<args->CF_mag<<"\n";
	std::cout<<"cont_sim: "<<args->cont_sim<<"\n";
}

SimulationArgs parseCommandLine(int argc, char* argv[])
{
	std::unordered_map<std::string, std::string> opts;

	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];

		if (arg == "-h" || arg == "--help")
		{
			printUsage();
			exit(EXIT_SUCCESS);
		}

		if (arg.size() > 2 && arg[0] == '-' && arg[1] == '-')
		{
			if (i + 1 >= argc)
			{
				std::cerr << "Error: " << arg << " requires a value.\n\n";
				printUsage();
				exit(EXIT_FAILURE);
			}
			opts[arg.substr(2)] = argv[++i];
		}
		else
		{
			std::cerr << "Error: unexpected argument '" << arg << "'.\n\n";
			printUsage();
			exit(EXIT_FAILURE);
		}
	}

	// Helpers — exit on missing required arg or bad type
	auto requireStr = [&](const std::string& key) -> std::string {
		auto it = opts.find(key);
		if (it == opts.end())
		{
			std::cerr << "Error: missing required argument --" << key << ".\n\n";
			printUsage();
			exit(EXIT_FAILURE);
		}
		return it->second;
	};

	auto requireDouble = [&](const std::string& key) -> double {
		std::string val = requireStr(key);
		try { return std::stod(val); }
		catch (...) {
			std::cerr << "Error: --" << key << " expects a number, got '" << val << "'.\n\n";
			printUsage();
			exit(EXIT_FAILURE);
		}
	};

	auto requireInt = [&](const std::string& key) -> int {
		std::string val = requireStr(key);
		try { return std::stoi(val); }
		catch (...) {
			std::cerr << "Error: --" << key << " expects an integer, got '" << val << "'.\n\n";
			printUsage();
			exit(EXIT_FAILURE);
		}
	};

	auto optDouble = [&](const std::string& key, double def) -> double {
		auto it = opts.find(key);
		if (it == opts.end()) return def;
		try { return std::stod(it->second); }
		catch (...) {
			std::cerr << "Error: --" << key << " expects a number, got '" << it->second << "'.\n\n";
			printUsage();
			exit(EXIT_FAILURE);
		}
	};

	auto optInt = [&](const std::string& key, int def) -> int {
		auto it = opts.find(key);
		if (it == opts.end()) return def;
		try { return std::stoi(it->second); }
		catch (...) {
			std::cerr << "Error: --" << key << " expects an integer, got '" << it->second << "'.\n\n";
			printUsage();
			exit(EXIT_FAILURE);
		}
	};

	SimulationArgs args;
	args.simtype      = requireStr("simtype");
	args.dt           = requireDouble("dt");
	args.damping      = requireDouble("damping");
	args.initial_temp = requireDouble("initial-temp");
	args.writestep    = requireInt("writestep");
	args.IN           = requireStr("in");
	args.infile       = requireStr("infile");
	args.OUT          = requireStr("out");
	args.CF_mag       = optDouble("cf-mag", 0.0);
	args.cont_sim     = static_cast<bool>(optInt("cont-sim", 0));

	if (args.simtype != "NVE" && args.simtype != "dampedMD" && args.simtype != "collapse_polymer")
	{
		std::cerr << "Error: --simtype must be NVE, dampedMD, or collapse_polymer;"
		          << " got '" << args.simtype << "'.\n\n";
		printUsage();
		exit(EXIT_FAILURE);
	}

	return args;
}

#endif
