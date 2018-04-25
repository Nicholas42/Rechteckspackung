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
	pack.read_inst_from(argv[1]);
	pack.read_sol_from(argv[2]);
	if(pack.init_bmp())
	{
		pack.draw_all_rectangles();
		pack.draw_all_pins();
		pack.write_bmp();
	}
}