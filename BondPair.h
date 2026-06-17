#ifndef BONDPAIR_H
#define BONDPAIR_H


#include <initializer_list>
#include <unordered_map>
#include "Bond.h"


class BondPair : public Bond
{
	public:

		double bond_length;

		BondPair(std::initializer_list<int> sphIDs, std::unordered_map<int, std::shared_ptr<Sphere>>& spheremap, double bond_stiffness, double blength) : Bond(sphIDs, spheremap, bond_stiffness), bond_length{blength} {}


		void print() override
		{
			std::cout<<"Bond sphere IDs:\n";

			for(auto sph : sphereIDs)
			{
				std::cout<<sph<<"\t";
			}
			std::cout<<"\nBond Stiffness: "<<stiffness<<"\n";
			std::cout<<"Equil Bond Length: "<<bond_length<<"\n";
			std::cout<<"\n\n";
		}


};


#endif

