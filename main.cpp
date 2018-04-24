#include <iostream>
#include <fstream>
#include <string>
#include "packing.h"

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
	if (argc < 2)
	{
		std::string s;
		std::cin >> s;
		std::ifstream fin(s);
		fin >> pack;
	}
	else
	{
		std::ifstream fin(argv[1]);
		fin >> pack;
	}

	std::cout << pack.is_valid() << std::endl;
	char c;
	std::cin >> c;
}