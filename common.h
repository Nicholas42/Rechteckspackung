/*
 * Just some definitions which can be used everywhere.
 */
#ifndef RECHTECKSPACKUNG_COMMON_H
#define RECHTECKSPACKUNG_COMMON_H

#include <list>
#include <cassert>
#include <string>
#include <iostream>
#include <stdexcept>

using pos = int;
using weight = int;

enum class rotation
{
    rotated_0 = 0,
    rotated_90 = 1,
    rotated_180 = 2,
    rotated_270 = 3,
    count = 4
};

enum class dimension
{
    x = 0,
    y = 1
};

static auto all_dimensions = {dimension::x, dimension::y};

inline std::string to_string(dimension dim)
{
    switch (dim)
    {
        case dimension::x:
            return "x";
        case dimension::y:
            return "y";
    }
}

struct certificate
{
    bool valid;
    int first;
    int second;

    certificate() :
            valid(true)
    {}

    certificate(int first_, int second_) :
            valid(false),
            first(first_),
            second(second_)
    {}
};


struct point
{
    pos x;
    pos y;

    // Sadly, std::optional is C++17, so we have to make do
    bool set = false;

    point() = default;

    point(pos x_, pos y_, bool set_) : x(x_), y(y_), set(set_)
    {}

    void swap()
    {
        std::swap(x, y);
    }

    /**
     * Returns the coordinate of the given dimension.
     * @param dim The coordinate to be returned
     * @param other Inverts the dimension (can be practical)
     * @return x if dim is dimension::x, y if dim is dimension::y, if other is true the choice is inverted
     */
    const pos &coord(dimension dim, bool other = false) const
    {
        switch (dim)
        {
            case dimension::x :
                return other ? y : x;
            case dimension::y :
                return other ? x : y;
            default:
				throw new std::invalid_argument("Invalid argurment: Unspecified value for dim");
		}
    }

    /**
     * Returns the coordinate of the given dimension.
     * @param dim The coordinate to be returned
     * @param other Inverts the dimension (can be practical)
     * @return x if dim is dimension::x, y if dim is dimension::y, if other is true the choice is inverted
     */
    pos &coord(dimension dim, bool other = false)
    {
        switch (dim)
        {
            case dimension::x :
                return other ? y : x;
            case dimension::y :
                return other ? x : y;
            default:
				throw new std::invalid_argument("Invalid argurment: Unspecified value for dim");
        }
    }
};

inline std::istream &operator>>(std::istream &in, point &p)
{
    for (dimension dim : all_dimensions)
    {
        if (!(in >> p.coord(dim)))
        {
            if (!p.set)
            {
                return in;
            }
            else
            {
                throw std::runtime_error("Invalid format for point, only one value provided.");
            }
        }

        p.set = true;
    }

    return in;
}

inline std::ostream &operator<<(std::ostream &out, const point &p)
{
    out << p.x << " " << p.y;
    return out;
}

#endif //RECHTECKSPACKUNG_COMMON_H
