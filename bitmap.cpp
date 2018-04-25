#include "bitmap.h"

/**
 * We want to write a lot of pixels...
 */
std::ostream &operator<<(std::ostream &out, const pixel &p)
{
    out << p.blue << p.green << p.red;
    return out;
}

/**
 * Puts the pixel p (aka the color) at position (x,y).
 */
void bitmap::put_pixel(int x, int y, const pixel &p)
{
    data.at(x + y*width) = p;
}

/**
 * Takes an int an stores it in the passed char array. 
 * WARNING: This should probably be done better, but I don't know how.
 */
void int_to_char_arr(px * to, int from)
{
    for(size_t i = 0; i < sizeof(int); i++)
    {
        to[i] = (from >> i*8) ;
    }
}

void bitmap::write()
{
    // I just loooooove this line. And it is necessary. And this is the problem.
    assert(sizeof(int) == 4);

    std::fstream file (filename, std::fstream::out | std::fstream::trunc | std::fstream::binary);

    std::array<px, HEADER_SIZE> header = BMP_HEADER;

    // Start: BMP Magic

    // BMP need a line width divisible by 4.
    const int extrabytes = (4 - ((width * 3) % 4))%4;
    const pixel empty{0,0,0};
    const int size_data = width * height * 3 + height * extrabytes;
    const int size_all = size_data + sizeof(header);
    
    int_to_char_arr(header.data() + WIDTH_POS, width);
    int_to_char_arr(header.data() + HEIGHT_POS, height);
    int_to_char_arr(header.data() + DATA_SIZE_POS, size_data);
    int_to_char_arr(header.data() + BYTE_SIZE_POS, size_all);

    for(auto c : header)
    {
        file << c;
    }

    // End: BMP Magic

    for(int i = 0; i < height; i++)
    {
        for(int j = i*width; j < (i+1)*width; j++)
        {
            file << data.at(j);
        }
        for(int k = 0; k < extrabytes; k++)
        {
            file << empty;
        }
    }
}

/**
 * Draws the edges of the specified rectangle with the specified color.
 * Applys scaling.
 */
void bitmap::draw_rectangle(int x_min, int x_max, int y_min, int y_max, pixel color)
{
    assert(x_min <= x_max && y_min <= y_max);

    x_min *= scaling;
    x_max *= scaling;
    y_min *= scaling;
    y_max *= scaling;

    for(int i = x_min; i <= x_max; i++)
    {
        put_pixel(i, y_min, color);
        put_pixel(i, y_max, color);
    }
    for(int i = y_min; i <= y_max; i++)
    {
        put_pixel(x_min, i, color);
        put_pixel(x_max, i, color);
    }
}

/**
 * Fills the interior of the specified rectangle with the specified color.
 * Applys scaling.
 */
void bitmap::fill_rectangle(int x_min, int x_max, int y_min, int y_max, pixel color)
{
    assert(x_min <= x_max && y_min <= y_max);

    x_min *= scaling;
    x_max *= scaling;
    y_min *= scaling;
    y_max *= scaling;

    for(int x = x_min + 1; x < x_max; x++)
    {
        for(int y = y_min + 1; y < y_max; y++)
        {
            put_pixel(x, y, color);
        }
    }
}

/**
 *  Draws a point. The size is more than one pixel for visibility reasons.
 */
void bitmap::draw_point(int x, int y, pixel color)
{
    x *= scaling;
    y *= scaling;

    for(int i = std::max(0, x - 3); i < std::min(width - 1, x + 3); i++)
    {
        for(int j = std::max(0, y - 3); j < std::min(height - 1, y + 3); j++)
        {
            put_pixel(i, j, color);
        }
    }
}

bool bitmap::valid(int width, int height)
{
    return height * width * 3 <= MAX_FILE_SIZE;
}