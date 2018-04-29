#include "rectangle.h"

/**
 * Checks if the rectangle is already placed.
 */
bool rectangle::placed() const
{
    return x != -1 && y != -1;
}

/**
 * Returns the x-value of the rightmost edge of the rectangle.
 * Observes rotation. Only works for already placed rectangles. 
 */
pos rectangle::x_max() const
{
    assert(placed());
    if(rot == rotated_0 || rot == rotated_180)
    {
        return x + width;
    }
    else
    {
        return x +height;
    }
}

/**
 * Checks if the rectangle contains the specified x-coordinate. 
 * Observes rotation. Only works for already placed rectangles. 
 * The right edge is not considered part of the rectangle.
 */
bool rectangle::contains_x(const pos to_check) const
{
    assert(placed());
    return x <= to_check && to_check < x_max();
}

/**
 * Returns the y-value of the uppermost edge of the rectangle.
 * Observes rotation. Only works for already placed rectangles.
 * The upper edge is not considered part of the rectangle.
 */
pos rectangle::y_max() const
{
    assert(placed());
    if(rot == rotated_0 || rot == rotated_180)
    {
        return y + height;
    }
    else
    {
        return y + width;
    }
}

/**
 * Checks if the rectangle contains the specified y-coordinate. 
 * Observes rotation. Only works for already placed rectangles.
 * The upper edge is not considered part of the rectangle.
 */
bool rectangle::contains_y(const pos to_check) const
{
    assert(placed());
    return y <= to_check && to_check < y_max();
}

/**
 * Checks if the rectangle intersects with the passed rectangle. 
 * A common border does not count as intersection. Only works for
 * already placed rectangles.
 */
bool rectangle::intersects(const rectangle &rect) const
{
    assert(placed() && rect.placed());

    return (contains_x(rect.x) || rect.contains_x(x)) &&
            (contains_y(rect.y) || rect.contains_y(y));
}

/**
 * Checks if the rectangle is left of the passed rectangle. 
 * The relevant data is the position of the base points.
 */
bool rectangle::left_of(const rectangle &rect) const
{
    assert(placed() && rect.placed());

    return x < rect.x;
}

/**
 * Checks if the rectangle is beneath the passed rectangle. 
 * The relevant data is the position of the base points.
 */
bool rectangle::beneath(const rectangle &rect) const
{
    assert(placed() && rect.placed());

    return y < rect.y;
}

/**
 * Standard operator used for sorting. We use vertical order as default.
 */
bool rectangle::operator<(const rectangle &rect) const
{
    if(y == rect.y)
    {
        return id < rect.id;
    }
    else
    {
        return beneath(rect);
    }
}

/**
 * Rotates the rectangle by the passed rotation. This does NOT set the rotation but rather 
 * increment it by the passed rotation.
 */
void rectangle::rotate(const rotation rotate)
{
    rot = static_cast<rotation>((rot + rotate)%rotation::count);
}

/**
 * A static wrapper for the left-of-comparison.
 */
bool rectangle::compare(const rectangle &left, const rectangle &right)
{
    return left.left_of(right);
}

/**
 * Returns the absolut position of a pin on this rectangle. Flipping and
 * rotation are observed. Only works on already placed rectangles and if
 * the pin belongs to the rectangle.
 */
std::pair<pos, pos> rectangle::get_pin_position(const pin &p) const
{
    assert(id == p.index);

    pos pin_x = p.x, pin_y = p.y;
    if(flipped)
    {
        pin_x = width - pin_x;
    }

    
    pos tmp_x;
    // This probably is faster than calculating sinus and cosinus
    // TODO: check my formulas
    switch(rot)
    {
        case rotated_0:
            break;
        case rotated_90:
            tmp_x = pin_x;
            pin_x = height - pin_y;
            pin_y = tmp_x;
            break;
        case rotated_180:
            pin_x = width - pin_x;
            pin_y = height - pin_y;
            break;
        case rotated_270:
            tmp_x = pin_x;
            pin_x = pin_y;
            pin_y = width - tmp_x;
            break;
        default:
            assert(false);
    }

    return std::make_pair(pin_x + x, pin_y + y);
}

rectangle rectangle::intersection(const rectangle &other) const
{
    rectangle ret;
    ret.x = std::max(x, other.x);
    ret.width = std::min(x_max(), other.x_max()) - ret.x;
    ret.y = std::max(y, other.y);
    ret.height = std::min(y_max(), other.y_max()) - ret.y;

    return ret;
}

/**
 * Outputs the rectangle. Does not end the line. Only works for already placed rectangles. 
 */
std::ostream &operator<<(std::ostream &out, const rectangle &rect)
{    
    // Assert that rectangle is already placed
    assert(rect.placed()); 

    out << rect.x << " ";

    if(rect.rot == rotated_0 || rect.rot == rotated_180)
    {
        // The rectangle is not rotated
        out << rect.x + rect.width << " ";
        out << rect.y << " ";
        out << rect.y + rect.height << " ";
    }
    else
    {
        // The rectangle is rotated, so height and width are interchanged
        out << rect.x + rect.height << " ";
        out << rect.y << " ";
        out << rect.y + rect.width << " ";
    }

    out << rect.flipped << " ";
    out << rect.rot;

    return out;
}

/**
 * Reads a rotation.
 */
std::istream &operator>> (std::istream &in, rotation &rot)
{
    int temp;
    in >> temp;

    rot = static_cast<rotation>(temp);
    
    return in;
}

/**
 * Reads a rectangle, works for blockages and rectangles (unplaced and placed).
 */
std::istream &operator>> (std::istream &in, rectangle &rect)
{
    if(in.peek() == 'B')
    {
        // We read a blockage
        rect.blockage = true;
        in.ignore();
        
        pos x_max = 0;
        pos y_max = 0;
        in >> rect.x;
        in >> x_max;
        in >> rect.y;
        in >> y_max;
        
        rect.width = x_max - rect.x;
        rect.height = y_max - rect.y;
    }
    else
    {
        // We read a rectangle
        pos first, second;

		if (!(in >> first))
		{
			return in;
		}
        in >> second;

        // Now we check if we reached the end of the line:
        if(in.peek() == '\n')
        {
            // So we read width and height of an unplaced rectangle
            rect.width = first;
            rect.height = second;
        }
        else
        {
            // So we are reading a placed rectangle
            rect.x = first;

            pos y_max;

            in >>  rect.y;
            in >> y_max;
            
            in >> rect.flipped;
            in >> rect.rot;

            if(rect.rot == rotated_0 || rect.rot == rotated_180)
            {
                // The rectangle is not rotated
                rect.width = second - rect.x;
                rect.height = y_max - rect.y;
            }
            else
            {
                // The rectangle is rotated, so height and width are interchanged
                rect.width = y_max - rect.y;
                rect.height = second - rect.x;
            }
        }
    }
	// Check that xmax > xmin and ymax > ymin
	if (rect.width < 0 || rect.height < 0)
	{
        // TODO: Maybe say what rectangle is the problem?
		throw std::runtime_error("Impossible rectangle dimensions");
	}

    return in;
}