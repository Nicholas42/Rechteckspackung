#include "net.h"

std::istream &operator>> (std::istream &in, pin &p)
{
    in >> p.index;
    in >> p.x;
    in >> p.y;

    return in;
}

std::istream &operator>> (std::istream &in, net &n)
{
    //TODO: Maybe we want to assert, that the stream starts with Net
    //TODO: Maybe we want to clear n.pin_list

    // We need to ignore the Net string
    in.ignore(3, ' ');

    in >> n.net_weight;

    if(!in)
    {
        // There is nothing left to read
        return in;
    }
    
    pin p;
    while(in >> p)
    {
        n.pin_list.push_back(p);
    }

    in.clear();

    return in;
}

std::ostream &operator<< (std::ostream &out, const pin &p)
{
    out << p.index << " ";
    out << p.x << " ";
    out << p.y;

    return out;
}

std::ostream &operator<< (std::ostream &out, const net &n)
{
    out << n.net_weight << std::endl;
    for(auto p: n.pin_list)
    {
        out << p << std::endl;
    }

    return out;
}