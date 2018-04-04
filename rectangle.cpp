#include "rectangle.h"

std::ostream &operator<<(std::ostream &out, const rectangle &rect)
{    
    // Assert that rectangle is already placed
    assert(rect.x != -1 && rect.y != -1); 

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
        in >> rect.width;
        in >> rect.height;
    }
    return in;
}
