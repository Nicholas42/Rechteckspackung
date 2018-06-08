/*
 * Just some definitions which can be used everywhere.
 */
#ifndef RECHTECKSPACKUNG_COMMON_H
#define RECHTECKSPACKUNG_COMMON_H

#include <list>

using pos = int;
using weight = int;

enum class rotation
{
    rotated_0 = 0,
    rotated_90 = 1,
    rotated_180 = 2,
    rotated_270 = 3,
    count = 4
};

enum class dimension
{
    x = 0,
    y = 1,
    count = 2
};

#endif //RECHTECKSPACKUNG_COMMON_H
