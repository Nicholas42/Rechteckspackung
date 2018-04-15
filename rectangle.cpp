#include "rectangle.h"

/**
 * Checks if the rectangle is already placed.
 */
bool rectangle::placed() const
{
    return x != -1 && y != -1;
}

/**
 * Checks if the interior of the rectangle contains the specified x-coordinate. Observes 
 * rotation. Only works for already placed rectangles.
 */
bool rectangle::contains_x(pos to_check) const
{
    assert(placed());
    if(rot == rotated_0 || rot == rotated_180)
    {
        return (x < to_check) && (to_check < x + width);
    }
    else
    {
        return (x < to_check) && (to_check < x + height);
    }
}

/**
 * Checks if the interior of the rectangle contains the specified y-coordinate. Observes 
 * rotation. Only works for already placed rectangles.
 */
bool rectangle::contains_y(pos to_check) const
{
    assert(placed());
    if(rot == rotated_0 || rot == rotated_180)
    {
        return (y < to_check) && (to_check < y + height);
    }
    else
    {
        return (y < to_check) && (to_check < y + width);
    }
}

/**
 * Checks if the rectangle intersects with the passed rectangle. A common border does not 
 * count as intersection. Only works for already placed rectangles.
 */
bool rectangle::intersects(const rectangle &rect) const
{
    assert(placed() && rect.placed());

    return (contains_x(rect.x) || rect.contains_x(x)) &&
            (contains_y(rect.y) || rect.contains_y(y));
}

/**
 * Checks if the rectangle is left of the passed rectangle. The relevant data is the position of 
 * the base points.
 */
bool rectangle::leftOf(const rectangle &rect) const
{
    assert(placed() && rect.placed());

    return x < rect.x;
}

/**
 * Checks if the rectangle is beneath the passed rectangle. The relevant data is the position of 
 * the base points.
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
    return beneath(rect);
}

/**
 * Rotates the rectangle by the passed rotation. This does NOT set the rotation but rather 
 * increment it passed rotation.
 */
void rectangle::rotate(rotation rotate)
{
    rot = static_cast<rotation>(rot + rotate);
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

        // Assert that xmax > xmin and ymax > ymin
        assert(rect.width > 0 && rect.height > 0);
    }
    else
    {
        // We read a rectangle
        pos first, second;

        in >> first;
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

        assert(rect.width >= 0 && rect.height >= 0);
    }
    return in;
}
