#ifndef BITMAP_H
#define BITMAP_H

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <array>
#include <cassert>
#include "rectangle.h"

typedef unsigned char px;

constexpr int HEADER_SIZE = 54;

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

constexpr int WIDTH_POS = 18; // width
constexpr int HEIGHT_POS = 22; // height
constexpr int DATA_SIZE_POS = 34; // width * height
constexpr int BYTE_SIZE_POS = 2; // width * height + 54

struct pixel
{
    px blue = 255;
    px green = 255;
    px red = 255;

    pixel ()
    {}

    pixel(px blue_, px green_, px red_):
        blue(blue_),
        green(green_),
        red(red_)
        {}
};

const pixel BLUE(255, 0, 0);
const pixel GREEN(0, 255, 0);
const pixel RED(0, 0, 255);
const pixel WHITE(255, 255, 255);
const pixel BLACK(0, 0, 0);



struct bitmap 
{
    const std::string filename;
    const int width;
    const int height;
    std::vector<pixel> data;
    const int scaling;
    bool written = false;

    bitmap(std::string filename_,
        int width_,
        int height_,
        int scaling_ = 1):
        filename(filename_),
        width(width_*scaling_),
        height(height_*scaling_),
        data(height*width),
        scaling(scaling_)
    {}

    void put_pixel(int x, int y, const pixel &p);
    void write();

    void draw_rectangle(int x_min, int x_max, int y_min, int y_max, pixel color);
    void fill_rectangle(int x_min, int x_max, int y_min, int y_max, pixel color);
};

std::ostream &operator<< (std::ostream &out, const pixel &p);

#endif //BITMAP_H