#include "rectangle.h"

/**
 * Checks if the rectangle is already placed.
 */
bool rectangle::placed() const
{
    return base.set;
}

/**
 * Checks if the rectangle intersects with the passed rectangle. 
 * A common border does not count as intersection. Only works for
 * already placed rectangles.
 */
bool rectangle::intersects(const rectangle &rect) const
{
    assert(placed() && rect.placed());

    bool ret = true;

    for (dimension dim : all_dimensions)
    {
        ret &= contains(rect.get_pos(dim, false), dim) || rect.contains(get_pos(dim, false), dim);
    }

    return ret;
}

/**
 * Checks if the rectangle is left of the passed rectangle. 
 * The relevant data is the position of the base points.
 */
bool rectangle::smaller(const rectangle &rect, dimension dim) const
{
    assert(placed() && rect.placed());

    return get_pos(dim, false) < rect.get_pos(dim, false);
}

/**
 * Standard operator used for sorting. We use vertical order as default.
 */
bool rectangle::operator<(const rectangle &rect) const
{
    if (get_pos(dimension::y, false) == rect.get_pos(dimension::y, false))
    {
        return id < rect.id;
    }
    else
    {
        return smaller(rect, dimension::y);
    }
}

/**
 * Rotates the rectangle by the passed rotation. This does NOT set the rotation but rather 
 * increment it by the passed rotation.
 */
void rectangle::rotate(const rotation rotate)
{
    rot = static_cast<rotation>(((int) rot + (int) rotate) % (int) rotation::count);
}

/**
 * A static wrapper for the left-of-comparison.
 */
bool rectangle::compare(const rectangle &left, const rectangle &right)
{
    return left.smaller(right, dimension::x);
}

/**
 * Returns the relative position of a pin on this rectangle. Flipping and
 * rotation are observed. Only works if the pin belongs to the rectangle.
 * The rectangle does not need to be already placed.
 * @param p The pin which position shall be returned. Has to be on this rectangle.
 * @return The relative position of the pin as a pair of pos.
 */
point rectangle::get_relative_pin_position(const pin &p) const
{
    assert(id == p.index);

    point pin_point = base;
    if (flipped)
    {
        pin_point.x = size.x - pin_point.x;
    }

    pos tmp_x;
    // This probably is faster than calculating sinus and cosinus
    // TODO: check my formulas
    switch (rot)
    {
        case rotation::rotated_0:
            break;
        case rotation::rotated_90:
            tmp_x = pin_point.x;
            pin_point.x = size.y - pin_point.y;
            pin_point.y = tmp_x;
            break;
        case rotation::rotated_180:
            pin_point.x = size.x - pin_point.x;
            pin_point.y = size.y - pin_point.y;
            break;
        case rotation::rotated_270:
            tmp_x = pin_point.x;
            pin_point.x = pin_point.y;
            pin_point.y = size.x - tmp_x;
            break;
        default:
            assert(false);
    }

    return pin_point;
}

rectangle rectangle::intersection(const rectangle &other) const
{
    rectangle ret;

    for (dimension dim : all_dimensions)
    {
        ret.base.coord(dim) = std::max(get_pos(dim, false), other.get_pos(dim, false));
        ret.size.coord(dim) = std::min(get_max(dim), other.get_max(dim));
    }

    return ret;
}

/**
 * Returns the absolute position of a pin on this rectangle. Flipping and
 * rotation are observed. Only works on already placed rectangles and if
 * the pin belongs to the rectangle.
 * @param p The pin which position shall be returned. Has to be on this rectangle.
 * @return The absolute position of the pin as a pair of pos.
 */
point rectangle::get_absolute_pin_position(const pin &p) const
{
    assert(placed());
    point pin_point = get_relative_pin_position(p);
    for (dimension dim : all_dimensions)
    {
        pin_point.coord(dim) += get_pos(dim, false);
    }
    return pin_point;
}

/**
 * Returns the relative position of a pin on this rectangle in the given
 * dimension. Flipping and rotation are observed. Only works on already
 * placed rectangles and if the pin belongs to the rectangle.
 * @param p The pin which position shall be returned. Has to be on this rectangle.
 * @param dim The dimension of the position to return.
 * @return The relative position of the pin in this dimension.
 */
pos rectangle::get_relative_pin_position(const pin &p, dimension dim) const
{
    return get_relative_pin_position(p).coord(dim);
}

/**
 * Getter for position in specified dimension. Only works on already placed rectangles.
 * @param dim The dimension of the position to return
 * @return The position of the rectangle in dimension dim
 */
pos rectangle::get_pos(dimension dim, bool other) const
{
    assert(placed());
    return base.coord(dim, other);
}

/**
 * Returns dimension in the given dimension (I think this is semantically correct).
 * Observes rotation. Rectangle does not need to be placed.
 * @param dim The dimension of the dimension to return
 * @return Either width or height
 */
pos rectangle::get_dimension(dimension dim) const
{
    return size.coord(dim, rotated());
}

pos rectangle::get_max(dimension dim, bool other) const
{
    assert(placed());
    return base.coord(dim, other) + size.coord(dim, rotated() ^ other);
}

bool rectangle::contains(const pos to_check, dimension dim) const
{
    assert(placed());
    return base.coord(dim) <= to_check && to_check < size.coord(dim, rotated());
}

bool rectangle::rotated() const
{
    return rot == rotation::rotated_90 || rot == rotation::rotated_270;
}

bool rectangle::contains_x(const pos to_check) const
{
    return contains(to_check, dimension::x);
}

bool rectangle::contains_y(const pos to_check) const
{
    return contains(to_check, dimension::y);
}

bool rectangle::contains(const point p) const
{
    for (dimension dim : all_dimensions)
    {
        if (!contains(p.coord(dim), dim))
        {
            return false;
        }
    }
    return true;
}

point rectangle::get_max_point() const
{
    assert(placed());

    point p;
    for (dimension dim: all_dimensions)
    {
        p.coord(dim) = get_pos(dim, false) + get_max(dim);
    }

    p.set = true;
    return p;
}

/**
 * Outputs the rectangle. Does not end the line. Only works for already placed rectangles. 
 */
std::ostream &operator<<(std::ostream &out, const rectangle &rect)
{
    // Assert that rectangle is already placed
    assert(rect.placed());

    for (dimension dim : all_dimensions)
    {
        out << rect.get_pos(dim) << " " << rect.get_max(dim) << " ";
    }

    out << rect.flipped << " ";
    out << (int) rect.rot;

    return out;
}

/**
 * Reads a rotation.
 */
std::istream &operator>>(std::istream &in, rotation &rot)
{
    int temp;
    in >> temp;

    rot = static_cast<rotation>(temp);

    return in;
}

/**
 * Reads a rectangle, works for blockages and rectangles (unplaced and placed).
 */
std::istream &operator>>(std::istream &in, rectangle &rect)
{
    if (in.peek() == 'B')
    {
        // We read a blockage
        rect.blockage = true;
        in.ignore();

        for (dimension dim : all_dimensions)
        {
            in >> rect.base.coord(dim);
            in >> rect.size.coord(dim);

            rect.size.coord(dim) -= rect.base.coord(dim);
        }
    }
    else
    {
        point p;
        if (!(in >> p))
        {
            return in;
        }

        // Now we check if we reached the end of the line:
        if (in.peek() == '\n')
        {
            // So we read width and height of an unplaced rectangle
            rect.size = p;
        }
        else
        {
            // So we are reading a placed rectangle
            rect.base.set = true;
            rect.base.x = p.x;

            pos y_max;

            in >> rect.base.y;
            in >> y_max;

            rect.size.x = p.y - rect.base.x;
            rect.size.y = y_max - rect.base.y;

            if (!(in >> rect.flipped))
            {
                // We read the chip base rectangle

            }
            else
            {
                in >> rect.rot;

                if (rect.rotated())
                {
                    rect.size.swap();
                }
            }
        }

        if (rect.size.x < 0 || rect.size.y < 0)
        {
            throw std::runtime_error("Invalid rectangle size.");
        }
    }

    // Check that the rectangle has a positive size
    for (dimension dim : all_dimensions)
    {
        if (rect.size.coord(dim) < 0)
        {
            throw std::runtime_error("Impossible rectangle dimensions in rectangle " + std::to_string(rect.id));
        }
    }
    return in;
}