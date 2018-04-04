#ifndef NET_H
#define NET_H

#include <iostream>
#include <vector>
#include <cassert>

typedef int pos;
typedef int weight;
//TODO: move typedefs to some overinclude

struct pin 
{
    pos x;
    pos y;

    int index; // index of the corresponding rectangle, -1 if fixed pin
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