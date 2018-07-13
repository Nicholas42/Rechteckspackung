#ifndef NET_H
#define NET_H

#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include "common.h"

struct pin 
{
    point position;

    int index; // index of the corresponding rectangle, -1 if fixed pin

    /**
     * Returns the position of the pin in the given dimension. This is always the postion relative to the unmodified
     * rectangle. To obtain the relative or absolute position of the pin respecting rotation and flipping of the
     * rectangle, use the functions in the rectangle class.
     * @param dim The dimension which is queried.
     * @return The position in the specified dimension.
     */
    pos get_pos(dimension dim) const;
};

struct net 
{
    weight net_weight;
    std::vector<pin> pin_list;
    size_t index;
};

std::istream &operator>> (std::istream &in, pin &p);
std::ostream &operator<< (std::ostream &out, const pin &p);

std::istream &operator>> (std::istream &in, net &n);
std::ostream &operator<< (std::ostream &out, const net &n);



#endif // NET_H