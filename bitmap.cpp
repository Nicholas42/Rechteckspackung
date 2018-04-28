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
void bitmap::put_pixel(int32_t x, int32_t y, const pixel &p)
{
    data.at(x + y*width) = p;
}

/**
 * Takes an int an stores it in the passed char array. 
 * WARNING: This should probably be done better, but I don't know how.
 */
void int_to_char_arr(px * to, int32_t from)
{
    for(size_t i = 0; i < sizeof(int32_t); i++)
    {
        to[i] = (from >> i*8) ;
    }
}

void bitmap::write()
{
    std::fstream file (filename, std::fstream::out | std::fstream::trunc | std::fstream::binary);

    std::array<px, HEADER_SIZE> header = BMP_HEADER;

    // Start: BMP Magic

    // BMP need a line width divisible by 4.
    const int32_t extrabytes = (4 - ((width * 3) % 4))%4;
    const pixel empty{0,0,0};
    const int32_t size_data = width * height * 3 + height * extrabytes;
    const int32_t size_all = size_data + sizeof(header);
    
    int_to_char_arr(header.data() + WIDTH_POS, width);
    int_to_char_arr(header.data() + HEIGHT_POS, height);
    int_to_char_arr(header.data() + DATA_SIZE_POS, size_data);
    int_to_char_arr(header.data() + BYTE_SIZE_POS, size_all);

    for(auto c : header)
    {
        file << c;
    }

    // End: BMP Magic

    for(int32_t i = 0; i < height; i++)
    {
        for(int32_t j = i*width; j < (i+1)*width; j++)
        {
            file << data.at(j);
        }
        for(int32_t k = 0; k < extrabytes; k++)
        {
            file << empty;
        }
    }
}

/**
 * Draws the edges of the specified rectangle with the specified color.
 * Applys scaling.
 */
void bitmap::draw_rectangle(int32_t x_min, int32_t x_max, int32_t y_min, int32_t y_max, pixel color)
{
    assert(x_min <= x_max && y_min <= y_max);

    x_min *= scaling;
    x_max *= scaling;
    y_min *= scaling;
    y_max *= scaling;

    for(int32_t i = x_min; i <= x_max; i++)
    {
        put_pixel(i, y_min, color);
        put_pixel(i, y_max, color);
    }
    for(int32_t i = y_min; i <= y_max; i++)
    {
        put_pixel(x_min, i, color);
        put_pixel(x_max, i, color);
    }
}

/**
 * Fills the interior of the specified rectangle with the specified color.
 * Applys scaling.
 */
void bitmap::fill_rectangle(int32_t x_min, int32_t x_max, int32_t y_min, int32_t y_max, pixel color)
{
    assert(x_min <= x_max && y_min <= y_max);

    x_min *= scaling;
    x_max *= scaling;
    y_min *= scaling;
    y_max *= scaling;

    for(int32_t x = x_min + 1; x < x_max; x++)
    {
        for(int32_t y = y_min + 1; y < y_max; y++)
        {
            put_pixel(x, y, color);
        }
    }
}

/**
 *  Draws a point. The size is more than one pixel for visibility reasons.
 */
void bitmap::draw_point(int32_t x, int32_t y, pixel color)
{
    x *= scaling;
    y *= scaling;

    for(int32_t i = std::max(0, x - 3); i < std::min(width - 1, x + 3); i++)
    {
        for(int32_t j = std::max(0, y - 3); j < std::min(height - 1, y + 3); j++)
        {
            put_pixel(i, j, color);
        }
    }
}

bool bitmap::valid(int32_t width, int32_t height)
{
    int64_t long_width = static_cast<int64_t> (width);
	int64_t long_height = static_cast<int64_t> (height);
    return (long_height * long_width * 3) <= MAX_FILE_SIZE;
}