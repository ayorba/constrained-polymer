#ifndef RESIDUE_H
#define RESIDUE_H

#include <memory>
#include <vector>
#include <unordered_map>
#include "Sphere.h"
#include "BondPair.h"


class Residue
{

	public:

		std::shared_ptr<Sphere> backbone;
		std::vector<std::shared_ptr<Sphere> > side_chain;
		std::vector<BondPair> bonds;


		// Constructor
		Residue(){};
		Residue(std::shared_ptr<Sphere>& bb, std::vector<std::shared_ptr<Sphere> >& sc) : backbone{bb}, side_chain{sc} {}


		void addBackBone(std::shared_ptr<Sphere> bb)
		{
			backbone = bb;
		}

		void addSideChain(std::shared_ptr<Sphere> sc)
		{
			side_chain.push_back(sc);
		}
		void addSideChain(std::vector<std::shared_ptr<Sphere> > sc)
		{
			for(std::size_t i=0; i<sc.size(); ++i)
			{
				side_chain.push_back(sc[i]);
			}
		}

		void addBond(int sphere1ID, int sphere2ID, std::unordered_map<int, std::shared_ptr<Sphere>>& spheremap, double bond_stiffness, double sphere1diam, double sphere2diam)
		{	
			bonds.push_back(BondPair({sphere1ID, sphere2ID}, spheremap, bond_stiffness, 0.5*(sphere1diam + sphere2diam)));
		}
		void addBond(int sphere1ID, int sphere2ID, std::unordered_map<int, std::shared_ptr<Sphere>>& spheremap, double bond_stiffness, double equil_bond_length)
		{	
			bonds.push_back(BondPair({sphere1ID, sphere2ID}, spheremap, bond_stiffness, equil_bond_length));
		}


		void print() const
		{

			backbone->print();

			for(auto sphere : side_chain)
			{
				sphere->print();
			}

		}

};


#endif

