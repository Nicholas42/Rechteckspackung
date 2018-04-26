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
#include "bitmap.h"

struct rect_ptr_compare
{
	bool operator() (rectangle *r, rectangle *l) const;
};

typedef std::set<rectangle *, rect_ptr_compare> sweepline;


class packing
{
private:
	std::vector<rectangle> rect_list;
	std::vector<net> net_list;
	bitmap bmp;
	std::string base_filename;

	pos x_min;
	pos x_max;
	pos y_min;
	pos y_max;

public:
	std::pair<int, int> is_valid();

	void read_sol_from(std::string filename);
	void read_inst_from(std::string filename);

	void draw_all_rectangles();
	void draw_all_pins();
	void write_bmp();

	bool init_bmp();

	friend std::ostream & operator<< (std::ostream &out, const packing &rect);
};

#endif // !PACKING_H
