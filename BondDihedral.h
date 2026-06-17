#ifndef BONDDIHEDRAL_H
#define BONDDIHEDRAL_H


#include <initializer_list>
#include <unordered_map>
#include "Bond.h"


class BondDihedral : public Bond
{
	public:

		double current_angle;	

		BondDihedral(std::initializer_list<int> sphIDs, std::unordered_map<int, std::shared_ptr<Sphere>>& spheremap, double bond_stiffness, double ca) : Bond(sphIDs, spheremap, bond_stiffness), current_angle{ca} {}


		void print() override
		{
			std::cout<<"Bond sphere IDs:\n";

			for(auto sph : sphereIDs)
			{
				std::cout<<sph<<"\t";
			}
			std::cout<<"\nBond Stiffness: "<<stiffness<<"\n";
			std::cout<<"Current Dihedral Angle: "<<current_angle<<"\n";
			std::cout<<"\n\n";
		}


};


#endif

