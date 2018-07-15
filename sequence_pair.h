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

	/**
	 * Creates a new sequence pair of the form ((1, 2, ..., n), (1, 2, ..., n)).
	 * @param length The length of the sequence pair, n in the example above.
	 */
	explicit sequence_pair (size_t length):
            positive_locus(length),
            negative_locus(length)
    {
        std::iota(positive_locus.begin(), positive_locus.end(), 0);
        std::iota(negative_locus.begin(), negative_locus.end(), 0);
    }

	/**
	 * Places rectangles according to this sequence pair. The placment will might be incomplete if
	 * it is impossible to place the rectangles this way in the given area.
	 * @param pack The packing which will be modified.
	 * @return True if this was succesful, false if rectangles were out of bounds.
	 */
	bool apply_to(packing &pack) const;
};

std::ostream &operator<<(std::ostream &out, const sequence_pair &);
#endif // !SEQUENCE_PAIR_H
