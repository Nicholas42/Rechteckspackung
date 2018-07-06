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
#include <sstream>
#include <numeric>
#include <tuple> // tie
#include "common.h"
#include "rectangle.h"
#include "net.h"
#include "bitmap.h"
#include "sequence_pair.h"
#include "min_cost_flow.h"

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

class rectangle_iterator;

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
    const net &get_net(size_t index) const;
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

    weight compute_netlength_optimal(const sequence_pair &list);

    rectangle_iterator get_iter();

    friend std::ostream &operator<<(std::ostream &out, const packing &rect);
};

class rectangle_iterator
{
private:
    std::vector<rectangle> &_rect_list;
    bool _at_end;
public:
    explicit rectangle_iterator (std::vector<rectangle> &rect_list_):
            _rect_list(rect_list_),
            _at_end(false)
    {}

    rectangle_iterator &operator++();
    explicit operator bool() const;
};

#endif // !PACKING_H
