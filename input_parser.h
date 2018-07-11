#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <algorithm>
#include <string>
#include <iostream>
#include <cmath>
#include "packing.h"

class input_parser
{
private:
	std::string get_option(char** begin, char** end, const std::string & option);
	bool get_switch(char** begin, char** end, const std::string & option);
	packing read_packing(std::string filename);
public:
	void parse(int argc, char* argv[]);
	void print_help();
	void optimize_bounding(packing & pack, unsigned int optimality, bool bitmap);
	void optimize_wirelength(packing & pack, unsigned int optimality, bool bitmap);
};

#endif // !INPUT_PARSER_H
