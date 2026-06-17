#include <fstream>
#include "MD.h"


// Write out a configuration file that can be loaded back in through MD::loadConfig()


void MD::writeFiles(int step, bool overwrite_files, bool write_vis_file, bool write_traj_file)
{

	std::ofstream output;

	// final_config.xyzr is always overwritten
	// temp_rg_Etot.txt is always appended
	std::string config_file;
	std::string vis_file;
	std::string final_config_file = "final_config.xyzr";
	std::string damping_progress_file = "temp_rg_Etot.txt";
	std::string traj_file = "traj.xyzr";


	if(overwrite_files)
	{
		config_file = "config_file.txt";
		vis_file = "xyzr_-1_povray.txt";
	}
	else
	{
		config_file = "config_"+std::to_string(step)+".txt";
		vis_file = "xyzr_"+std::to_string(step)+"_povray.txt";
	}
	

	// Choose to overwrite or append config_file
	std::ios_base::openmode mode = std::ios_base::app;
	if(overwrite_files)
	{
		mode = std::ios::out;
	}


	// Write files
	output.open(OUT+config_file, mode);
	if(output.is_open())
	{

		for(const auto& res : sim->residues)
		{
			auto* backbone = res.backbone.get();

			// Write out atoms 
			// resID, atomID, x, y, z, diameter, mass, velx, vely, velz, Fx, Fy, Fz

			// Backbone sphere
			output<<"ATOM,"<<backbone->residueID<<","<<backbone->atomID<<","<<backbone->position.x<<","<<backbone->position.y<<","<<backbone->position.z<<","<<backbone->diameter<<","<<backbone->mass<<","<<backbone->velocity.x<<","<<backbone->velocity.y<<","<<backbone->velocity.z<<","<<backbone->force.x<<","<<backbone->force.y<<","<<backbone->force.z<<"\n";

			// Side chain spheres
			for(const auto& sph_ptr : res.side_chain)
			{
				auto* sph = sph_ptr.get();

				output<<"ATOM,"<<sph->residueID<<","<<sph->atomID<<","<<sph->position.x<<","<<sph->position.y<<","<<sph->position.z<<","<<sph->diameter<<","<<sph->mass<<","<<sph->velocity.x<<","<<sph->velocity.y<<","<<sph->velocity.z<<","<<sph->force.x<<","<<sph->force.y<<","<<sph->force.z<<"\n";
			}

			// Write out intra-residue bonds
			// atomID, atomID, stiffness, equil_length
			for(const auto& bond : res.bonds)
			{	
				output<<"INTRA_RESIDUE,"<<bond.sphereIDs[0]<<","<<bond.sphereIDs[1]<<","<<bond.stiffness<<","<<bond.bond_length<<"\n";
			}
			if(res.bonds.empty())
			{
				output<<"INTRA_RESIDUE\n";
			}

		}


		// Write out inter-residue bonds
		// atomID, atomID, stiffness, equil_length
		for(const auto& bond : sim->backbone_pairs)
		{
			output<<"INTER_RESIDUE,"<<bond.sphereIDs[0]<<","<<bond.sphereIDs[1]<<","<<bond.stiffness<<","<<bond.bond_length<<"\n";
		}


		// Write out fixed bond angles
		// atomID, atomID, atomID, stiffness, fixed_angle, current_angle
		for(const auto& bond : sim->bond_angles)
		{
			output<<"BOND_ANGLE,"<<bond.sphereIDs[0]<<","<<bond.sphereIDs[1]<<","<<bond.sphereIDs[2]<<","<<bond.stiffness<<","<<bond.fixed_angle<<","<<bond.current_angle<<"\n";

		}


		// Write out dihedral angles
		// atomID, atomID, atomID, atomID, stiffness, current_angle
		for(const auto& bond : sim->bond_dihedrals)
		{
			output<<"BOND_DIHEDRAL,"<<bond.sphereIDs[0]<<","<<bond.sphereIDs[1]<<","<<bond.sphereIDs[2]<<","<<bond.sphereIDs[3]<<","<<bond.stiffness<<","<<bond.current_angle<<"\n";
		}


		output<<"END";

	}
	output.close();


	// final_config file (used in packing faction code and others)

	output.open(OUT+final_config_file, std::ios::out);
	if(output.is_open())
	{
		output<<"atomID\tx\ty\tz\tradius\tresidueID\n";

		// Backbone atoms
		for(const auto& res : sim->residues)
		{
			auto* backbone = res.backbone.get();

			output<<backbone->atomID<<"\t"<<backbone->position.x<<"\t"<<backbone->position.y<<"\t"<<backbone->position.z<<"\t"<<0.5*backbone->diameter<<"\t"<<backbone->residueID<<"\n";

			// Side chain atoms
			for(const auto& sph_ptr : res.side_chain)
			{
				auto* sph = sph_ptr.get();

				output<<sph->atomID<<"\t"<<sph->position.x<<"\t"<<sph->position.y<<"\t"<<sph->position.z<<"\t"<<0.5*sph->diameter<<"\t"<<sph->residueID<<"\n";

			}

		}

	}
	output.close();


	// File to keep track of relaxation of packing
	output.open(OUT+damping_progress_file, std::ios_base::app);
	if(output.is_open())
	{
		if(step==0)
		{
			output<<"temp\tradius_of_gyration\tEtot\tFmag_max\n";
		}
		else
		{
			output<<temperature<<"\t"<<sim->Rg<<"\t"<<interman->Etot<<"\t"<<interman->Fmag_max<<"\n";
		}
	}
	output.close();


	// Blender Visualization
	if(write_vis_file)
	{
		Vector3D com = sim->getCOM();

		output.open(OUT+vis_file, mode);
		std::vector<double> bbcolor = {0.184, 0.31, 0.471, 1};
		std::vector<double> sccolor = {0, 0.596, 0, 1};


		if(output.is_open())
		{
			for(const auto& res : sim->residues)
			{
		
				output<<res.backbone->position.x<<","<<res.backbone->position.y<<","<<res.backbone->position.z<<","<<0.5*res.backbone->diameter<<","<<com.x<<","<<com.y<<","<<com.z<<","<<640<<","<<bbcolor[0]<<","<<bbcolor[1]<<","<<bbcolor[2]<<","<<bbcolor[3]<<",\n";

				for(const auto& sph : res.side_chain)
				{
					output<<sph->position.x<<","<<sph->position.y<<","<<sph->position.z<<","<<0.5*sph->diameter<<","<<com.x<<","<<com.y<<","<<com.z<<","<<640<<","<<sccolor[0]<<","<<sccolor[1]<<","<<sccolor[2]<<","<<sccolor[3]<<",\n";

				}
			}
		}

		output.close();
	}


	// OVITO trajectory (XYZR)
	if(write_traj_file)
	{
		std::ios_base::openmode traj_mode = std::ios_base::app;
		if(step == 0)
		{
			// Start a fresh trajectory each run.
			traj_mode = std::ios::out;
		}

		output.open(OUT + traj_file, traj_mode);
		if(output.is_open())
		{
			const std::size_t N = sim->spheres.size();
			output << N << "\n";
			output << "Timestep " << step << "\n";

			for(const auto& sph_ptr : sim->spheres)
			{
				const auto* sph = sph_ptr.get();
				output << sph->position.x << " "
					   << sph->position.y << " "
					   << sph->position.z << " "
					   << 0.5 * sph->diameter << "\n";
			}
		}
		output.close();
	}


}


