#ifndef SEQUENCE_PAIR_H
#define SEQUENCE_PAIR_H

#include <list>
#include <numeric>
#include <algorithm>
#include <iostream>
#include "common.h"

class sequence_pair
{
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
};

class sequence_pair_iterator
{
private:
    sequence_pair _sp;
    bool _at_end;
public:
    explicit sequence_pair_iterator(size_t length):
        _sp(length),
        _at_end(false)
    {}

    sequence_pair_iterator &operator++();
    explicit operator bool() const;
    sequence_pair &operator*();
};

std::ostream &operator<<(std::ostream &out, const sequence_pair &);
#endif // !SEQUENCE_PAIR_H
