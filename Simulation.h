#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>
#include <initializer_list>
#include <unordered_map>
#include "Sphere.h"
#include "Residue.h"
#include "BondPair.h"
#include "BondAngle.h"
#include "BondDihedral.h"
#include "PairHash.h"


class Simulation
{

	public:

	const int dim = 3;	

	int Nres;
	int Natoms;

	double Rg;
	double max_diameter;
	double min_diameter;

	std::vector<std::shared_ptr<Sphere> > spheres;
	std::vector<Residue> residues;
	std::vector<BondPair> backbone_pairs;
	// For nonbonded_pairs hold only the index of the included bonds in vector all_non_perm_bond_pairs
	std::vector<int> nonbonded_pairs;
	std::vector<BondAngle> bond_angles;
	std::vector<BondDihedral> bond_dihedrals;

	// Every permanent bond
	// This includes backbone_pairs and intra-residue bonds
	// This is used to make all_non_perm_bond_pairs
	// Keep track of permanent bonded pairs to create complete list of non-permanent pairs
	std::unordered_set<std::pair<int, int>, PairHash> bonded_pairs;


	// Every possible non-permanent bond pair
	// This is iterated over to remake Verlet list
	std::vector<std::shared_ptr<BondPair> > all_non_perm_bond_pairs;
	int num_non_perm_bond_pairs;


	// Constructor
	Simulation(){}

	Vector3D getCOM();
	void computeRg();


	void addSpheres(std::shared_ptr<Sphere>& sph)
	{
		spheres.push_back(sph);
	}
	void addSpheres(std::vector<std::shared_ptr<Sphere>>& sphs)
	{
		for(std::size_t i=0; i<sphs.size(); ++i)
		{
			spheres.push_back(sphs[i]);
		}
	}


	// Permanent Bonds //

	// Permanent backbone pairs of sphere IDs
	void addBackBonePair(int sphere1ID, int sphere2ID, std::unordered_map<int, std::shared_ptr<Sphere>>& spheremap, double bond_stiffness, double sphere1diam, double sphere2diam)
	{	
		backbone_pairs.push_back(BondPair({sphere1ID, sphere2ID}, spheremap, bond_stiffness, 0.5*(sphere1diam + sphere2diam)));
	}
	void addBackBonePair(int sphere1ID, int sphere2ID, std::unordered_map<int, std::shared_ptr<Sphere>>& spheremap,  double bond_stiffness, double equil_bond_length)
	{	
		backbone_pairs.push_back(BondPair({sphere1ID, sphere2ID}, spheremap, bond_stiffness, equil_bond_length));
	}


	// Permanent bonds between triplets of sphere IDs that make up bond angle
	void addBondAngle(int sphere1ID, int sphere2ID, int sphere3ID, std::unordered_map<int, std::shared_ptr<Sphere>>& spheremap,  double bond_stiffness, double fixed_angle, double current_angle)
	{	
		bond_angles.push_back(BondAngle({sphere1ID, sphere2ID, sphere3ID}, spheremap, bond_stiffness, fixed_angle, current_angle));
	}

	// Permanent bonds between quadruplets of sphere IDs that make up dihedral angle
	void addBondDihedral(int sphere1ID, int sphere2ID, int sphere3ID, int sphere4ID, std::unordered_map<int, std::shared_ptr<Sphere>>& spheremap,  double bond_stiffness, double current_angle)
	{	
		bond_dihedrals.push_back(BondDihedral({sphere1ID, sphere2ID, sphere3ID, sphere4ID}, spheremap, bond_stiffness, current_angle));
	}


	void addResidues(Residue& newres)
	{
		residues.push_back(newres);
	}
	void addResidues(std::vector<Residue>& newres)
	{
		for(std::size_t i=0; i<newres.size(); ++i)
		{
			residues.push_back(newres[i]);
		}
	}


	void findMinMaxDiam()
	{
		max_diameter = 0.0;
		min_diameter = 1e6;
		for(const auto& sph : spheres)
		{
			const auto& diam = sph->diameter;
			if(diam > max_diameter)
			{
				max_diameter = diam;
			}
			if(diam < min_diameter)
			{
				min_diameter = diam;
			}
		}
	}


	// Make the complete list of all possible nonbonded pairs that can interact
	// This only needs to be made once at the start of the simulation
	void makeNonPermanentBondPairs()
	{
		std::unordered_map<int, std::shared_ptr<Sphere> > sphereMap;
		int atomIDa, atomIDb;
		bool spha_found;
		bool sphb_found;
		std::shared_ptr<Sphere> spha_ptr;
		std::shared_ptr<Sphere> sphb_ptr;

		for(int i=0; i<Natoms-1; ++i)
		{
			for(int j=i+1; j<Natoms; ++j)
			{
				atomIDa = spheres[i]->atomID;
				atomIDb = spheres[j]->atomID;
		
				sphereMap.clear();

				spha_found = 0;
				sphb_found = 0;
				for(const auto& sph : spheres)
				{					
					if(sph->atomID==atomIDa)
					{
						spha_ptr = sph;
						spha_found = 1;
					}
					else if(sph->atomID==atomIDb)
					{
						sphb_ptr = sph;
						sphb_found = 1;
					}
					if(spha_found && sphb_found)
					{
						sphereMap[atomIDa] = spha_ptr;
						sphereMap[atomIDb] = sphb_ptr;
						break;
					}
				}

				if(atomIDa < atomIDb)
				{
					if(bonded_pairs.find({atomIDa,atomIDb}) == bonded_pairs.end())
					{
						all_non_perm_bond_pairs.push_back( std::make_shared<BondPair>(std::initializer_list<int>{atomIDa, atomIDb}, sphereMap, 1, 0.5*(spha_ptr->diameter + sphb_ptr->diameter)) );

					}
				}
				else
				{
					if(bonded_pairs.find({atomIDb,atomIDa}) == bonded_pairs.end())
					{
						all_non_perm_bond_pairs.push_back( std::make_shared<BondPair>(std::initializer_list<int>{atomIDb, atomIDa}, sphereMap, 1, 0.5*(sphb_ptr->diameter + spha_ptr->diameter)) );

					}
				}

			}
		}

		// Record the permanent size of the number of non-permanent bond pairs
		num_non_perm_bond_pairs = all_non_perm_bond_pairs.size();
	}


};


#endif

