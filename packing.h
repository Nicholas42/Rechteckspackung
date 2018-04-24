#ifndef PACKING_H
#define PACKING_H

#include <vector>
#include <set>
#include "rectangle.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <cctype>

class packing
{
private:
	std::vector<rectangle> rect_list;
public:
	bool is_valid();

	friend std::ostream & operator<< (std::ostream &out, const packing &rect);
	friend std::istream & operator>> (std::istream &in, packing &rect);
};

#endif // !PACKING_H
