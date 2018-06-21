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

    pos get_pos(dimension dim) const;
};

struct net 
{
    weight net_weight;
    std::vector<pin> pin_list;
};

std::istream &operator>> (std::istream &in, pin &p);
std::ostream &operator<< (std::ostream &out, const pin &p);

std::istream &operator>> (std::istream &in, net &n);
std::ostream &operator<< (std::ostream &out, const net &n);



#endif // NET_H