#ifndef PACKING_H
#define PACKING_H

#include <vector>
#include <set>
#include <list>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <cctype>
#include <fstream>
#include <numeric>
#include <tuple> // tie
#include "common.h"
#include "rectangle.h"
#include "net.h"
#include "bitmap.h"
#include "sequence_pair.h"

struct rect_ptr_compare
{
    bool operator()(rectangle *r, rectangle *l) const;
};

struct rect_ind_compare
{
    const std::vector<rectangle> &elements;

    bool operator()(size_t first, size_t second) const;
};

using sweepline = std::set<rectangle *, rect_ptr_compare>;

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

    pos get_x_min() const;
    pos get_x_max() const;
    pos get_y_min() const;
    pos get_y_max() const;
    pos get_max_pos(dimension dim) const;
    pos get_min_pos(dimension dim) const;

    rectangle &get_rect(size_t index);
    net &get_net(size_t index);
    size_t get_num_rects() const;
    size_t get_num_nets() const;


    std::pair<int, int> is_valid() const;

    sequence_pair to_sequence_pair() const;

    void read_sol_from(const std::string filename);

    void read_inst_from(const std::string filename);

    void read_dimension_from_inst(const std::string filename);

    void draw_all_rectangles();
    void draw_all_pins();
    void draw_cert(const std::pair<int, int> cert);
    void write_bmp();

    bool init_bmp();

    friend std::ostream &operator<<(std::ostream &out, const packing &rect);
};

#endif // !PACKING_H
