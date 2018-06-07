#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <iostream>
#include <cassert>
#include <algorithm>
#include "net.h"
#include "common.h"


struct rectangle 
{
    pos x = -1;
    pos y = -1;
    pos width = 0;
    pos height = 0;
    int id;

    bool blockage = false;
    bool flipped = false;
    rotation rot = rotation::rotated_0;

    pos x_max() const;
    pos y_max() const;

    bool placed() const;
    bool contains_x(const pos to_check) const;
    bool contains_y(const pos to_check) const;
    bool intersects(const rectangle &rect) const;

    bool left_of(const rectangle &rect) const;
    bool beneath(const rectangle &rect) const;
    bool operator<(const rectangle &rect) const;

    std::pair<pos, pos> get_pin_position(const pin &p) const;
    
    void rotate(const rotation rotate);
    rectangle intersection(const rectangle &other) const;

    static bool compare(const rectangle &left, const rectangle &right);
};

std::ostream &operator<< (std::ostream &out, const rectangle &rect);

std::istream &operator>> (std::istream &in, rectangle &rect);

#endif // RECTANGLE_H