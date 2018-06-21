#include "net.h"

std::istream &operator>> (std::istream &in, pin &p)
{
    if (!(in >> p.index))
    {
        return in;
    }

    for (dimension dim : all_dimensions)
    {
        in >> p.position.coord(dim);
    }
    return in;
}

std::istream &operator>> (std::istream &in, net &n)
{
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
