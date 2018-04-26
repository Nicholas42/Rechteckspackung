#include <iostream>
#include <fstream>
#include <string>
#include "packing.h"
#include "bitmap.h"

//packing read_interactive()
//{
//	packing pack;
//	std::cout << "Interactive mode..." << std::endl;
//	if (!(std::cin >> pack))
//	{
//		std::cout << "Invalid format!";
//	}
//
//	return pack;
//}

int main(int argc, char *argv[])
{
	packing pack;
	if(argc < 3)
	{
		throw std::runtime_error("Insufficient arguments. Usage: ./programname instance_file solution_file");
	}

	pack.read_dimension_from_inst(argv[1]);
	pack.read_sol_from(argv[2]);

	std::pair<int, int> cert = pack.is_valid();

	if(cert.first == -1)
	{
		std::cout << "No intersections found." << std::endl;
	}
	else
	{
		std::cout << "Rectangles " << cert.first << " and " << cert.second << " intersect." << std::endl;
	}

	if(argc > 3 && strcmp(argv[3], "-b") == 0)
	{
		if(pack.init_bmp())
		{
			pack.draw_all_rectangles();
			pack.draw_all_pins();
			pack.draw_cert(cert);
			pack.write_bmp();
		}
		else
		{
			std::cout << "Instance is too big for a bitmap." << std::endl;
		}
	}
	else
	{
		std::cout << "Run again with flag -b for a beautiful bitmap" << std::endl;
	}
}