#ifndef BITMAP_H
#define BITMAP_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <array>
#include <cassert>
#include <cstdint>
#include "rectangle.h"

typedef unsigned char px;

constexpr int64_t MAX_FILE_SIZE = 10000000; //10 MB

constexpr int32_t HEADER_SIZE = 54;

constexpr std::array<px, HEADER_SIZE> BMP_HEADER = {/* Bitmapfileheader */
                     'B', 'M', //ASCII "BM"
                     0, 0, 0, 0, //size in byte
                     0, 0, 0, 0, //default 0
                     54, 0, 0, 0, //image data offset in byte
                     /* Bitmapinfoheader */
                     40, 0, 0, 0, //size of Bitmapinfoheader in byte
                     0, 0, 0, 0, //width in pxl
                     0, 0, 0, 0, //height in pxl
                     1, 0, //outdated
                     24, 0, //color depth
                     0, 0, 0, 0, //compression type
                     0x00, 0x0C, 0, 0, //size of image data
                     0, 0, 0, 0, //horizontal resolution, default 0
                     0, 0, 0, 0, //vertical resolution, default 0
                     0, 0, 0, 0, //color palette, if used
                     0, 0, 0, 0 //only if color palette used
};

constexpr int32_t WIDTH_POS = 18; // width
constexpr int32_t HEIGHT_POS = 22; // height
constexpr int32_t DATA_SIZE_POS = 34; // width * height
constexpr int32_t BYTE_SIZE_POS = 2; // width * height + 54

struct pixel
{
    px blue;
    px green;
    px red;

    pixel(px blue_, px green_, px red_):
        blue(blue_),
        green(green_),
        red(red_)
        {}

    pixel () : pixel(255, 255, 255)
    {}
};

const pixel BLUE{255, 0, 0};
const pixel GREEN{0, 255, 0};
const pixel RED{0, 0, 255};
const pixel WHITE{255, 255, 255};
const pixel BLACK{0, 0, 0};

struct bitmap
{
    std::string filename;
	int32_t width;
	int32_t height;
    std::vector<pixel> data;
	int32_t scaling;
    bool initialized;
    int32_t effective_width;

    bitmap() :
        initialized(false)
    {}

    bitmap(std::string &&filename_, int32_t width_, int32_t height_, int32_t scaling_ = 1) :
            filename(filename_),
            width(width_ * scaling_),
            height(height_ * scaling_ + 1),
            scaling(scaling_),
            initialized(true),
            effective_width((width + 4) / 4 * 4)
    {
        // I really don't want to plot inst10 by mistake
        assert(valid(width, height));
        data.resize(height * effective_width * 3, WHITE);
    }

    static bool valid(const int32_t width, const int32_t height);

    void put_pixel(const int32_t x,const int32_t y, const pixel &p);
    void write();

    void draw_rectangle(const rectangle &rect, const pixel &color);
    void fill_rectangle(const rectangle &rect, const pixel &color);
    void draw_point(const point &p, const pixel &color);
};

std::ostream &operator<< (std::ostream &out, const pixel &p);

#endif //BITMAP_H