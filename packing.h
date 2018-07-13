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
#include <stdexcept>
#include "common.h"
#include "rectangle.h"
#include "net.h"
#include "bitmap.h"
#include "sequence_pair.h"
#include "min_cost_flow.h"

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

using sweepline = std::set<size_t , rect_ind_compare>;

class rectangle_iterator;

class packing
{
private:
    std::vector<rectangle> _rect_list;
    std::vector<net> _net_list;
    bitmap _bmp;
    std::string _base_filename;
    rectangle _chip_base;

public:

    /**
     * Returns the rectangle which represents the base of the rectangle.
     * @return _chip_base
     */
    const rectangle &get_chip_base() const;

    /**
     * Returns the rectangle with the given index.
     * @param index The index of the rectangle to return.
     * @return The rectangle with this index, -1 means the chip base, so this works well with pins.
     */
    const rectangle &get_rect(int index) const;
    rectangle &get_rect(int index);

    /**
     * Returns the net with the given index.
     * @param index
     * @return
     */
    const net &get_net(size_t index) const;

    /**
     * Returns the number of rectangles of this instance.
     * @return _rect_list.size()
     */
    size_t get_num_rects() const;

    /**
     * Returns the number of nets of this instance.
     * @return _net_list.size()
     */
    size_t get_num_nets() const;

    void move_rect(int index, point pos);

    /**
     * Checks if this packing is valid, i.e. that no two different rectangles intersect.
     * @return A certificate, if the packing is valid, the valid member of the certificate is true. Otherwise it is
     * false and the certificate contains two rectangles which intersect.
     */
    const certificate is_valid() const;

    /**
     * Computes the total netlength of this packing.
     * @return The weighted sum of the half bounding box of all nets.
     */
    weight compute_netlength() const;

    /**
     * Calculates a sequence pair which fits to the current packing.
     * @return Such a sequence pair.
     */
    sequence_pair to_sequence_pair() const;

    /**
     * Reads a solution file and stores it in this packing.
     * @param filename The name of the solution file to read.
     */
    void read_sol_from(const std::string filename);

    /**
     * Reads an instance file and stores the instance in this packing.
     * @param filename The name of the file to read.
     */
    void read_inst_from(const std::string filename);

    /**
     * Reads just the dimensions of the chip base from an instance file.
     * @param filename The name of the file to read.
     */
    void read_dimension_from_inst(const std::string filename);

    /**
     * The drawing methods should be called in the following order, since they overwrite each other.
     * draw_all_rectangles()
     * draw_cert()
     * draw_all_nets()
     * draw_all_pins()
     */

    /**
     * Draws all rectangles of the packing to _bmp.
     */
    void draw_all_rectangles();

    /**
     * Draws a certificate, i.e. colors the two rectangles in a different color. Does nothing if the certificate is
     * valid
     * @param cert The certificate to draw.
     */
    void draw_cert(const certificate &cert);

    /**
     * Draws all pins of the packing to _bmp.
     */
    void draw_all_nets();

    /**
     * Draws all nets of the packing to _bmp.
     */
    void draw_all_pins();

    /**
     * Writes _bmp to _base_filename.bmp
     */
    void write_bmp();

    /**
     * Initializes _bmp. If the packing is to big, this will fail.
     * @return True if initialization was succesful, false otherwise.
     */
    bool init_bmp();

    /**
     * Computes a netlength optimal packing respecting this sequence pair. The rectangles of this packing will be
     * placed accordingly.
     * @param sp The sequence pair which gives the left-right and above-below restrictions.
     * @return The weight of the packing.
     */
    weight compute_netlength_optimal(const sequence_pair &sp);

    /**
     * Returns the area which is covered by all rectangles.
     * @return The area covered.
     */
    pos calculate_area();

    /**
     * Returns a rectangle_iterator which iterates over all possible rotations and flippings of the rectangles.
     * @param bounds_only For the bounding box optimality flipping is irrelevant and rotation by 180 degrees is the same
     * as no rotation. If this is true only 2 rotations per rectangle will be iterated.
     * @return A rectangle_iterator which does that.
     */
    rectangle_iterator get_iter(bool bounds_only);

    friend std::ostream &operator<<(std::ostream &out, const packing &rect);
};

class rectangle_iterator
{
private:
    std::vector<rectangle> &_rect_list;
    bool _at_end;
    bool _bounds_only;
public:
    explicit rectangle_iterator(std::vector<rectangle> &rect_list_, bool bounds_only_) :
            _rect_list(rect_list_),
            _at_end(false),
            _bounds_only(bounds_only_)
    {}

    rectangle_iterator &operator++();
    explicit operator bool() const;
};

#endif // !PACKING_H
