#ifndef PAIRHASH_H
#define PAIRHASH_H


// Hash function made to work with std::unordered_set<std::pair<int, int> >
// All pairs should have the small ID followed by the larger ID, e.g., (1, 2) and NOT (2, 1)


#include <unordered_set>
#include <utility> // std::pair
#include <functional> // std::hash


struct PairHash
{

	std::size_t operator()(const std::pair<int, int>& pair) const
	{
		std::size_t hash1 = std::hash<int>{}(pair.first);
		std::size_t hash2 = std::hash<int>{}(pair.second);
		return hash1 ^ (hash2 << 1);
	}


};


#endif

