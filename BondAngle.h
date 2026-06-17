#ifndef BONDANGLE_H
#define BONDANGLE_H


#include <initializer_list>
#include <unordered_map>
#include "Bond.h"


class BondAngle : public Bond
{
	public:

		double fixed_angle;
		double current_angle;	

		BondAngle(std::initializer_list<int> sphIDs, std::unordered_map<int, std::shared_ptr<Sphere>>& spheremap, double bond_stiffness, double fa, double ca) : Bond(sphIDs, spheremap, bond_stiffness), fixed_angle{fa}, current_angle{ca} {}


		void print()
		{
			std::cout<<"Bond sphere IDs:\n";

			for(auto sph : sphereIDs)
			{
				std::cout<<sph<<"\t";
			}
			std::cout<<"\nBond Stiffness: "<<stiffness<<"\n";
			std::cout<<"Fixed Bond Angle: "<<fixed_angle<<"\n";
			std::cout<<"Current Bond Angle: "<<current_angle<<"\n";
			std::cout<<"\n\n";
		}


};


#endif

