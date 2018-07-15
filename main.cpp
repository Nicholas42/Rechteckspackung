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

    return 0;
}