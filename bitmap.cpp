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
void bitmap::put_pixel(const int32_t x,const int32_t y, const pixel &p)
{
    data.at(x + y * effective_width) = p;
}

/**
 * Takes an int an stores it in the passed char array. 
 * WARNING: This should probably be done better, but I don't know how.
 */
void int_to_char_arr(px * to, const int32_t from)
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
    const int32_t size_data = data.size();
    const int32_t size_all = size_data + sizeof(header);

    int_to_char_arr(header.data() + WIDTH_POS, effective_width);
    int_to_char_arr(header.data() + HEIGHT_POS, height);
    int_to_char_arr(header.data() + DATA_SIZE_POS, size_data);
    int_to_char_arr(header.data() + BYTE_SIZE_POS, size_all);

    for(auto c : header)
    {
        file << c;
    }

    // End: BMP Magic

    for (int32_t i = 0; i < height; i++)
    {
        for (int32_t j = i * effective_width; j < (i + 1) * effective_width; j++)
        {
            file << data.at(j);
        }
    }
}

/**
 * Draws the edges of the specified rectangle with the specified color.
 * Applys scaling.
 */
void bitmap::draw_rectangle(const rectangle &rect, const pixel &color)
{
    for (dimension dim : all_dimensions)
    {
        for (int32_t i = scaling * rect.get_pos(dim, false); i <= rect.get_max(dim); ++i)
        {
            put_pixel(i, rect.get_pos(dim, true) * scaling, color);
            put_pixel(i, rect.get_max(dim, true) * scaling, color);
        }
    }
}

/**
 * Fills the interior of the specified rectangle with the specified color.
 * Applys scaling.
 */
void bitmap::fill_rectangle(const rectangle &rect, const pixel &color)
{
    for (int32_t x = rect.base.x * scaling + 1; x < rect.get_max(dimension::x) * scaling; x++)
    {
        for (int32_t y = rect.base.y * scaling + 1; y < rect.get_max(dimension::y) * scaling; y++)
        {
            put_pixel(x, y, color);
        }
    }
}

/**
 *  Draws a point. The size is more than one pixel for visibility reasons.
 */
void bitmap::draw_point(const point &p, const pixel &color)
{
    assert(p.set);
    int32_t x = scaling * p.x;
    int32_t y = scaling * p.y;

    for(int32_t i = std::max(0, x - 3); i < std::min(width - 1, x + 3); i++)
    {
        for(int32_t j = std::max(0, y - 3); j < std::min(height - 1, y + 3); j++)
        {
            put_pixel(i, j, color);
        }
    }
}

bool bitmap::valid(const int32_t width,const  int32_t height)
{
    const int64_t long_width = static_cast<const int64_t> (width);
	const int64_t long_height = static_cast<const int64_t> (height);
    return (long_height * long_width) <= MAX_FILE_SIZE/3;
}