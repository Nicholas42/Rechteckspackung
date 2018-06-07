#include <iostream>
#include <fstream>
#include "packing.h"

int main(int argc, char *argv[])
{
    packing pack;
    if (argc < 3)
    {
        throw std::runtime_error("Insufficient arguments. Usage: ./programname instance_file solution_file");
    }

    pack.read_dimension_from_inst(argv[1]);
    pack.read_sol_from(argv[2]);

    std::pair<int, int> cert = pack.is_valid();

    if (cert.first == -1)
    {
        std::cout << "No intersections found." << std::endl;
    } else
    {
        std::cout << "Rectangles " << cert.first << " and " << cert.second << " intersect." << std::endl;
    }

    auto list = pack.to_sequence_pair();
    auto i = list.first.begin();
    auto j = list.second.begin();

    for (; i != list.first.end() && j != list.second.end(); i++, j++)
    {
        std::cout << *i << " " << *j << std::endl;
    }

    if (argc > 3 && strcmp(argv[3], "-b") == 0)
    {
        if (pack.init_bmp())
        {
            pack.draw_all_rectangles();
            pack.draw_all_pins();
            pack.draw_cert(cert);
            pack.write_bmp();
        } else
        {
            std::cout << "Instance is too big for a bitmap." << std::endl;
        }
    } else
    {
        std::cout << "Run again with flag -b for a beautiful bitmap" << std::endl;
    }
}