#include "net.h"

std::istream &operator>> (std::istream &in, pin &p)
{
    if (!(in >> p.index))
    {
        return in;
    }

    if(!(in >> p.position))
    {
        throw std::runtime_error("Invalid format for net, pin position not specified.");
    }

    return in;
}

std::istream &operator>> (std::istream &in, net &n)
{
    n.pin_list.clear();
    std::string prefix;
    in >> prefix;

    if(prefix != "Net")
    {
        in.setstate(std::ios_base::failbit);
        return in;
    }
    in >> n.net_weight;
    
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
    out << p.position;

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

pos pin::get_pos(dimension dim) const
{
    return position.coord(dim);
}
