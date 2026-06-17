#ifndef BOND_H
#define BOND_H

#include <initializer_list>
#include <unordered_map>

// Hold sphere IDs of the spheres that make up the bond
// This may mean permanently bonded, as in the backbone spheres with side chain spheres,
// temporarily bonded, as in two spheres that happen to be close at the moment and interacting,
// or 3-4 spheres that make up a bond angle or dihedral angle. 


class Bond
{
	public:

		std::vector<int> sphereIDs;
		std::unordered_map<int, std::shared_ptr<Sphere> > sphereMap;
		double stiffness;

		Bond(std::initializer_list<int> sphIDs, std::unordered_map<int, std::shared_ptr<Sphere>>& spheremap, double stiff) : sphereIDs{sphIDs}, sphereMap{spheremap}, stiffness{stiff} {}

};


#endif

