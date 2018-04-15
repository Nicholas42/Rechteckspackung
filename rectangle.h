#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <iostream>
#include <cassert>


typedef int pos;

enum rotation
{
    min = 0,
    rotated_0 = 0,
    rotated_90 = 1,
    rotated_180 = 2,
    rotated_270 = 3,
    max = 3
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

    bool placed() const;
    bool contains_x(pos to_check) const;
    bool contains_y(pos to_check) const;
    bool intersects(const rectangle &rect) const;

    bool left_of(const rectangle &rect) const;
    bool beneath(const rectangle &rect) const;
    bool operator<(const rectangle &rect) const;
    
    void rotate(rotation rotate);

    static bool compare(const rectangle &left, const rectangle &right);
};

std::ostream &operator<< (std::ostream &out, const rectangle &rect);

std::istream &operator>> (std::istream &in, rectangle &rect);

#endif // RECTANGLE_H