#ifndef PLACEMENT_ITERATOR_H
#define PLACEMENT_ITERATOR_H

#include <algorithm>
#include <bitset>
#include <functional>
#include <vector>
#include "packing.h"
#include "rectangle.h"
#include "sequence_pair.h"

class rectangle_iterator
{
private:
	std::vector<std::reference_wrapper<rectangle>> _rect_list;
	bool _at_end;
	bool _bounds_only;
public:
	explicit rectangle_iterator(std::vector<std::reference_wrapper<rectangle>> rect_list_, bool bounds_only_) :
		_rect_list(rect_list_),
		_at_end(false),
		_bounds_only(bounds_only_)
	{}

	rectangle_iterator &operator++();
	explicit operator bool() const;
};

class placement_iterator
{
private:
	packing & _pack;
	size_t _optimality;
	bool _bounds_only;
	bool _at_end, _new_subset;
	rectangle_iterator _rect_it;
	sequence_pair _sp;
	std::vector<size_t> _positive_subset, _negative_subset;
	std::vector<std::pair<std::list<size_t>::iterator, std::list<size_t>::iterator>> _subset_positions;

	template<class It>
	bool _next_combination(It begin, It middle, It end);
	void _next_subset();

public:
	placement_iterator(packing & pack, size_t optimality, bool bounds_only);

	placement_iterator &operator++();
	operator bool() const;
	sequence_pair &operator*();
};

#endif // !PLACEMENT_ITERATOR_H
