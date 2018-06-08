#ifndef SEQUENCE_PAIR_H
#define SEQUENCE_PAIR_H

#include <list>
#include "common.h"

class sequence_pair
{
public:
	std::list<size_t> positive_locus, negative_locus;
};
#endif // !SEQUENCE_PAIR_H
