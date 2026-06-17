#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "MD.h"
#include "Residue.h"
#include "Sphere.h"
#include "Vector3D.h"


// Read in a configuration file (in the same format as writeFiles.cpp creates)


void MD::loadConfig()
{

	std::ifstream input;
	std::string line, label, token, delimiter = ",";
	std::string last_label="";
	std::vector<std::string> values;
	int resID=0, atomID;
	double diam,mass;
	Vector3D pos;
	Vector3D vel;
	Vector3D force;
	std::vector<std::shared_ptr<Sphere> > loadedspheres;
	std::vector<std::shared_ptr<Sphere> > newspheres;
	int sphaID, sphbID, sphcID, sphdID;
	bool spha_found, sphb_found, sphc_found, sphd_found;
	std::shared_ptr<Sphere> spha_ptr, sphb_ptr, sphc_ptr, sphd_ptr;
	std::unordered_map<int, std::shared_ptr<Sphere> > sphereMap;
	double stiffness, equil_length, fixed_angle, current_angle;
	Residue newres;
	int Nsph=0;


	// Handling single-atom residues
	int current_resID = -1; // track residue boundaries when INTRA_RESIDUE is absent
	auto finalizeBufferedResidue = [&]() {
		if(newspheres.empty()) return;
		// Add backbone and side chain spheres to residue
		newres.addBackBone(newspheres[0]);
		newspheres.erase(newspheres.begin());
		newres.addSideChain(newspheres); // empty OK for 1-atom residues
		newspheres.clear();
		sim->addResidues(newres);
		newres = Residue();
		current_resID = -1;
	};


	int atom_counter = 0;
	int res_counter = 0;


	input.open(IN+input_filename);
	std::size_t linepos = 0;
	while(std::getline(input,line))
	{

		label = line.substr(0, line.find(delimiter));


		// Parse line
		values.clear();
		while((linepos = line.find(delimiter)) != std::string::npos)
		{
			line.erase(0, line.find(delimiter) + delimiter.length());
			token = line.substr(0, line.find(delimiter));
			values.push_back(token);
		}


		// If finished reading in residue, add to Simulation()
		if(last_label=="INTRA_RESIDUE" && label!=last_label) 
		{
			sim->addResidues(newres);
			newres = Residue();
			current_resID = -1;
		}

		// If we are leaving an ATOM block without ever seeing INTRA_RESIDUE for that residue,
		// we still need to finalize and add the residue (covers ATOM-only residues, including
		// single-atom residues, when the next label is INTER_RESIDUE/BOND_ANGLE/BOND_DIHEDRAL/END/etc.).
		if(last_label=="ATOM" && label!="ATOM" && label!="INTRA_RESIDUE")
		{
			finalizeBufferedResidue();
		}


		if(label=="ATOM")
		{
			int next_resID = std::stoi(values[0]);

			// If INTRA_RESIDUE is absent, residues are delimited by residueID changes inside the ATOM section.
			// Finalize the previous residue when we encounter the first ATOM of the next residue.
			if(current_resID != -1 && next_resID != current_resID)
			{
				finalizeBufferedResidue();
			}
			current_resID = next_resID;

			resID = next_resID;
			res_counter = resID+1;

			atomID = std::stoi(values[1]);

			pos.x = std::stod(values[2]);
			pos.y = std::stod(values[3]);
			pos.z = std::stod(values[4]);

			diam = std::stod(values[5]);
			mass = std::stod(values[6]);

			if(values.size() > 7)
			{
				vel.x = std::stod(values[7]);
				vel.y = std::stod(values[8]);
				vel.z = std::stod(values[9]);

				force.x = std::stod(values[10]);
				force.y = std::stod(values[11]);
				force.z = std::stod(values[12]);

				newspheres.push_back(std::make_shared<Sphere>(resID, atomID, diam, mass, pos, vel, force)); 	
			}
			else
			{
				newspheres.push_back(std::make_shared<Sphere>(resID, atomID, diam, mass, pos)); 	
			}

			loadedspheres.push_back(newspheres.back());
			Nsph++;
			
			// Create sphere object
			sim->addSpheres(newspheres.back());

			atom_counter++;
		}
		else if(label=="INTRA_RESIDUE")
		{
			if(last_label=="ATOM")
			{
				// Add backbone and side chain spheres to residue
				newres.addBackBone(newspheres[0]);
				newspheres.erase(newspheres.begin());			
				newres.addSideChain(newspheres);
				newspheres.clear();
			}

			if(values.size()>1)
			{
				sphaID = std::stoi(values[0]);
				sphbID = std::stoi(values[1]);
				stiffness = std::stod(values[2]);
				equil_length = std::stod(values[3]);

				// Add bond to residue
				sphereMap.clear();

				// If atomIDs match the index in the spheres vector, it's easy
				if(Nsph>=sphaID && Nsph>=sphbID && loadedspheres[sphaID]->atomID==sphaID && loadedspheres[sphbID]->atomID==sphbID)
				{

					sphereMap[sphaID] = loadedspheres[sphaID];
					sphereMap[sphbID] = loadedspheres[sphbID];

					newres.addBond(sphaID, sphbID, sphereMap, stiffness, equil_length);
					sim->bonded_pairs.insert({sphaID, sphbID});
				}
				// otherwise we need to find the correct sphere object...
				else
				{
					spha_found = 0;
					sphb_found = 0;
					for(const auto& sph : loadedspheres)
					{					
						if(sph->atomID==sphaID)
						{
							spha_ptr = sph;
							spha_found = 1;
						}
						else if(sph->atomID==sphbID)
						{
							sphb_ptr = sph;
							sphb_found = 1;
						}
						if(spha_found && sphb_found)
						{
							sphereMap[sphaID] = spha_ptr;
							sphereMap[sphbID] = sphb_ptr;
							break;
						}
					}

					newres.addBond(sphaID, sphbID, sphereMap, stiffness, equil_length);
					sim->bonded_pairs.insert({sphaID, sphbID});

				}

			}

		}
		else if(label=="INTER_RESIDUE")
		{
			sphaID = std::stoi(values[0]);
			sphbID = std::stoi(values[1]);
			stiffness = std::stod(values[2]);
			equil_length = std::stod(values[3]);

			sphereMap.clear();

			// If atomIDs match the index in the spheres vector, it's easy
			if(Nsph>=sphaID && Nsph>=sphbID && loadedspheres[sphaID]->atomID==sphaID && loadedspheres[sphbID]->atomID==sphbID)
			{
				sphereMap[sphaID] = loadedspheres[sphaID];
				sphereMap[sphbID] = loadedspheres[sphbID];

				sim->addBackBonePair(sphaID, sphbID, sphereMap, stiffness, equil_length);
				sim->bonded_pairs.insert({sphaID, sphbID});

			}
			// otherwise we need to find the correct sphere object...
			else
			{
				spha_found = 0;
				sphb_found = 0;
				for(const auto& sph : loadedspheres)
				{					
					if(sph->atomID==sphaID)
					{
						spha_ptr = sph;
						spha_found = 1;
					}
					else if(sph->atomID==sphbID)
					{
						sphb_ptr = sph;
						sphb_found = 1;
					}
					if(spha_found && sphb_found)
					{
						sphereMap[sphaID] = spha_ptr;
						sphereMap[sphbID] = sphb_ptr;
						break;
					}

				}

				sim->addBackBonePair(sphaID, sphbID, sphereMap, stiffness, equil_length);
				sim->bonded_pairs.insert({sphaID, sphbID});
			}

		}
		else if(label=="BOND_ANGLE")
		{
			sphaID = std::stoi(values[0]);
			sphbID = std::stoi(values[1]);
			sphcID = std::stoi(values[2]);
			stiffness = std::stod(values[3]);
			fixed_angle = std::stod(values[4]);
			current_angle = std::stod(values[5]);

			sphereMap.clear();

			// If atomIDs match the index in the spheres vector, it's easy
			if(Nsph>=sphaID && Nsph>=sphbID && Nsph>=sphcID && loadedspheres[sphaID]->atomID==sphaID && loadedspheres[sphbID]->atomID==sphbID && loadedspheres[sphcID]->atomID==sphcID)
			{
				sphereMap[sphaID] = loadedspheres[sphaID];
				sphereMap[sphbID] = loadedspheres[sphbID];
				sphereMap[sphcID] = loadedspheres[sphcID];

				sim->addBondAngle(sphaID, sphbID, sphcID, sphereMap, stiffness, fixed_angle, current_angle);
			}
			// otherwise we need to find the correct sphere object...
			else
			{
				spha_found = 0;
				sphb_found = 0;
				sphc_found = 0;
				for(const auto& sph : loadedspheres)
				{					
					if(sph->atomID==sphaID)
					{
						spha_ptr = sph;
						spha_found = 1;
					}
					else if(sph->atomID==sphbID)
					{
						sphb_ptr = sph;
						sphb_found = 1;
					}
					else if(sph->atomID==sphcID)
					{
						sphc_ptr = sph;
						sphc_found = 1;
					}

					if(spha_found && sphb_found && sphc_found)
					{
						sphereMap[sphaID] = spha_ptr;
						sphereMap[sphbID] = sphb_ptr;
						sphereMap[sphcID] = sphc_ptr;
						break;
					}
				}

				sim->addBondAngle(sphaID, sphbID, sphcID, sphereMap, stiffness, fixed_angle, current_angle);
			}


		}
		else if(label=="BOND_DIHEDRAL")
		{
			sphaID = std::stoi(values[0]);
			sphbID = std::stoi(values[1]);
			sphcID = std::stoi(values[2]);
			sphdID = std::stoi(values[3]);
			stiffness = std::stod(values[4]);
			current_angle = std::stod(values[5]);

			sphereMap.clear();

			// If atomIDs match the index in the spheres vector, it's easy
			if(Nsph>=sphaID && Nsph>=sphbID && Nsph>=sphcID && Nsph>=sphdID && loadedspheres[sphaID]->atomID==sphaID && loadedspheres[sphbID]->atomID==sphbID && loadedspheres[sphcID]->atomID==sphcID && loadedspheres[sphdID]->atomID==sphdID)
			{
				sphereMap[sphaID] = loadedspheres[sphaID];
				sphereMap[sphbID] = loadedspheres[sphbID];
				sphereMap[sphcID] = loadedspheres[sphcID];
				sphereMap[sphdID] = loadedspheres[sphdID];

				sim->addBondDihedral(sphaID, sphbID, sphcID, sphdID, sphereMap, stiffness, current_angle);
			}
			// otherwise we need to find the correct sphere object...
			else
			{

				spha_found = 0;
				sphb_found = 0;
				sphc_found = 0;
				sphd_found = 0;
				for(const auto& sph : loadedspheres)
				{					
					if(sph->atomID==sphaID)
					{
						spha_ptr = sph;
						spha_found = 1;
					}
					else if(sph->atomID==sphbID)
					{
						sphb_ptr = sph;
						sphb_found = 1;
					}
					else if(sph->atomID==sphcID)
					{
						sphc_ptr = sph;
						sphc_found = 1;
					}
					else if(sph->atomID==sphdID)
					{
						sphd_ptr = sph;
						sphd_found = 1;
					}

					if(spha_found && sphb_found && sphc_found && sphd_found)
					{
						sphereMap[sphaID] = spha_ptr;
						sphereMap[sphbID] = sphb_ptr;
						sphereMap[sphcID] = sphc_ptr;
						sphereMap[sphdID] = sphd_ptr;
						break;
					}
				}

				sim->addBondDihedral(sphaID, sphbID, sphcID, sphdID, sphereMap, stiffness, current_angle);
			}

		}

		last_label = label;


	}

	// EOF flush: ensure the last residue is not dropped.
	// - last_label == "ATOM": residue had no INTRA_RESIDUE section, so spheres are buffered in newspheres.
	// - last_label == "INTRA_RESIDUE": residue exists in `newres`, but the usual label-change trigger never fires at EOF.
	if(last_label=="ATOM")
	{
		finalizeBufferedResidue();
	}
	else if(last_label=="INTRA_RESIDUE")
	{
		sim->addResidues(newres);
		newres = Residue();
		current_resID = -1;
	}

	std::cout<<"Natoms: "<<atom_counter<<"\n";
	std::cout<<"Nres: "<<res_counter<<"\n";

}


