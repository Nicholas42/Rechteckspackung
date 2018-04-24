#ifndef PACKING_H
#define PACKING_H

#include <vector>
#include <set>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <cctype>
#include <fstream>
#include "rectangle.h"
#include "net.h"

class packing
{
private:
	std::vector<rectangle> rect_list;
	std::vector<net> net_list;
public:
	bool is_valid();

	void read_sol_from(std::string filename);
	void read_inst_from(std::string filename);

	friend std::ostream & operator<< (std::ostream &out, const packing &rect);
};

#endif // !PACKING_H
