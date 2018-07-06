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

struct bounding_box
{
    point min;
    point max;

    pos half_circumference() const;

    void add_point(const point &p);

    rectangle to_rectangle() const;
};

using sweepline = std::set<rectangle *, rect_ptr_compare>;

class packing
{
private:
    std::vector<rectangle> rect_list;
    std::vector<net> net_list;
    bitmap bmp;
    std::string base_filename;
    rectangle chip_base;

public:

    const rectangle &get_chip_base() const;

    const rectangle &get_rect(int index) const;
    rectangle &get_rect(int index);
    net &get_net(size_t index);
    size_t get_num_rects() const;
    size_t get_num_nets() const;


    const certificate is_valid() const;
    weight compute_netlength() const;

    sequence_pair to_sequence_pair() const;

    void read_sol_from(const std::string filename);

    void read_inst_from(const std::string filename);

    void read_dimension_from_inst(const std::string filename);

    void draw_all_rectangles();
    void draw_all_pins();
    void draw_all_nets();
    void draw_cert(const certificate &cert);
    void write_bmp();

    bool init_bmp();

    friend std::ostream &operator<<(std::ostream &out, const packing &rect);
};

#endif // !PACKING_H
