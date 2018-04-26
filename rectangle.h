#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <iostream>
#include <cassert>
#include "net.h"


typedef int pos;

enum rotation
{
    rotated_0 = 0,
    rotated_90 = 1,
    rotated_180 = 2,
    rotated_270 = 3,
    count = 4
};

struct rectangle 
{
    pos x = -1;
    pos y = -1;
    pos width = 0;
    pos height = 0;
    int id;

    bool blockage = false;
    bool flipped = 0;
    rotation rot = rotated_0;

    pos x_max() const;
    pos y_max() const;

    bool placed() const;
    bool contains_x(pos to_check) const;
    bool contains_y(pos to_check) const;
    bool intersects(const rectangle &rect) const;

    bool left_of(const rectangle &rect) const;
    bool beneath(const rectangle &rect) const;
    bool operator<(const rectangle &rect) const;

    std::pair<pos, pos> get_pin_position(const pin &p) const;
    
    void rotate(rotation rotate);
    rectangle intersection(const rectangle &other) const;

    static bool compare(const rectangle &left, const rectangle &right);
    static bool compare1(const rectangle *left, const rectangle *right);
};

std::ostream &operator<< (std::ostream &out, const rectangle &rect);

std::istream &operator>> (std::istream &in, rectangle &rect);

#endif // RECTANGLE_H