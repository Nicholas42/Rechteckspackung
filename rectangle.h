#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <iostream>

typedef int pos;

enum rotation
{
    rotated_0 = 0,
    rotated_90 = 1,
    rotated_180 = 2,
    rotated_270 = 3
};

struct rectangle 
{
    pos x = -1;
    pos y = -1;
    pos width = 0;
    pos height = 0;

    bool blockage = false;
    bool flipped = 0;
    rotation rot = rotated_0;    
};

std::ostream& operator<< (std::ostream &out, const rectangle &rect);

std::istream& operator>> (std::istream &in, rectangle &rect);

int main()
{
    rectangle r;

    std::cin >> r;
    std::cout << r;
}

#endif // RECTANGLE_H