#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <optional>
#include <iostream>
#include <cassert>
#include <tuple> // tie
#include <algorithm>
#include "net.h"
#include "common.h"


struct rectangle 
{
    // Basepoint, may be not set
    point base;

    // Point contains width as x and height as y
    point size;
	int id;

    bool blockage = false;
    bool flipped = false;
    rotation rot = rotation::rotated_0;

    bool rotated() const;

    bool contains(const point p) const;
    bool contains(const pos to_check, dimension dim) const;
    bool contains_x(const pos to_check) const;
    bool contains_y(const pos to_check) const;

    pos get_dimension(dimension dim) const;

    pos get_pos(dimension dim, bool other = false) const;

    /**
    * Getter for the right or upper boundary of the rectangle. Only works on already placed rectangles.
    * Observes rotation.
    * @param dim The dimension of the boundary to return
    * @return Either the right or the upper boundary
    */
    pos get_max(dimension dim, bool other = false) const;

    point get_max_point() const;

    bool placed() const;
    bool intersects(const rectangle &rect) const;

    bool smaller(const rectangle &rect, dimension dim) const;
    bool operator<(const rectangle &rect) const;

    pos get_relative_pin_position(const pin &p, dimension dim) const;
    point get_relative_pin_position(const pin &p) const;
    point get_absolute_pin_position(const pin &p) const;
    
    void rotate(const rotation rotate);
    rectangle intersection(const rectangle &other) const;

    static bool compare(const rectangle &left, const rectangle &right);
};

std::ostream &operator<< (std::ostream &out, const rectangle &rect);

std::istream &operator>> (std::istream &in, rectangle &rect);

#endif // RECTANGLE_H