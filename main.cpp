#include <iostream>
#include <fstream>
#include "common.h"
#include "packing.h"
#include "input_parser.h"
#include "min_cost_flow.h"

int main(int argc, char *argv[])
{
	input_parser parser;
	parser.parse(argc, argv);

    /*packing pack;
    if (argc < 3)
    {
        throw std::runtime_error("Insufficient arguments. Usage: ./programname instance_file solution_file");
    }

    pack.read_inst_from(argv[1]);*/
    /*
    pack.read_sol_from(argv[2]);

    std::cout << pack << std::endl;
    std::cout << "Netlength: " << pack.compute_netlength() << std::endl;

    certificate cert = pack.is_valid();

    if (cert.valid)
    {
        std::cout << "No intersections found." << std::endl;
    } else
    {
        std::cout << "Rectangles " << cert.first << " and " << cert.second << " intersect." << std::endl;
    }

    auto list = pack.to_sequence_pair();
    list.positive_locus.reverse();
    auto i = list.positive_locus.begin();
    auto j = list.negative_locus.begin();

    for (; i != list.positive_locus.end() && j != list.negative_locus.end(); i++, j++)
    {
        std::cout << *i << " " << *j << std::endl;
    }

    if (argc > 3 && strcmp(argv[3], "-b") == 0)
    {
        if (pack.init_bmp())
        {
            pack.draw_all_rectangles();
            pack.draw_all_pins();
            pack.draw_all_nets();
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

    std::cout << "#Nets: " << pack.get_num_nets() << "; #Rects: " << pack.get_num_rects() << std::endl;
    for(size_t i = 0; i < pack.get_num_nets(); ++i)
    {
        std::cout << pack.get_net(i);
    }
     */

    /*packing best_pack;
    weight  value = std::numeric_limits<weight>::max();
    rectangle_iterator rect_it = pack.get_iter();
    size_t count = 0;

    do
    {
        std::cout << "ROT:   " << (int) pack.get_rect(0).rot << std::endl;
        sequence_pair_iterator sp_it(pack.get_num_rects());
        do
        {
            weight new_value = pack.compute_netlength_optimal(*sp_it);
            std::cout << *sp_it << std::endl;
            std::cout << new_value << std::endl;
            if (new_value < value)
            {
                best_pack = pack;
                value = new_value;
            }
            std::cout << ++count << std::endl;
        }
        while (++sp_it);
    } while (++rect_it);

    std::cout << "Value of placement: " << best_pack.compute_netlength() << std::endl;

    std::cout << best_pack;
    if(best_pack.init_bmp())
    {
        best_pack.draw_all_rectangles();
        best_pack.draw_all_pins();
        best_pack.draw_all_nets();
        best_pack.write_bmp();
    }*/
    return 0;
}