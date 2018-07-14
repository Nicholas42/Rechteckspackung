#ifndef SEQUENCE_PAIR_H
#define SEQUENCE_PAIR_H

#include <list>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include "common.h"
#include "packing.h"

class packing;

class sequence_pair
{
private:
	std::vector<pos> place_dimension(dimension dim, const packing & pack) const;

public:
	std::list<size_t> positive_locus, negative_locus;

	sequence_pair () = default;

	explicit sequence_pair (size_t length):
            positive_locus(length),
            negative_locus(length)
    {
        std::iota(positive_locus.begin(), positive_locus.end(), 0);
        std::iota(negative_locus.begin(), negative_locus.end(), 0);
    }

	bool apply_to(packing &pack);
};

std::ostream &operator<<(std::ostream &out, const sequence_pair &);
#endif // !SEQUENCE_PAIR_H
