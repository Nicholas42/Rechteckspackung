#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include "packing.h"
#include "placement_iterator.h"

class input_parser
{
private:
	std::string get_option(char** begin, char** end, const std::string & option);
	bool get_switch(char** begin, char** end, const std::string & option);
	packing read_packing(std::string filename);
public:
	/**
	 * Parses command line arguments and acts on them.
	 * @param argc The number of arguments
	 * @param argv An array with the arguments of length argc
	 */
	void parse(int argc, char* argv[]);
	void print_help();

	/**
	 * Finds a k-optimal placement regarding the area of the bounding rectangle for the given packing. 
	 * Writes the solution to the given path when there is one, writes error to console otherwise.
	 * @param pack The packing that should be placed.
	 * @param optimality k. If zero an optimal placement is searched.
	 * @param bitmap Indicates whether to output a bitmap of the best placement.
	 * @param output_file The path where to write the output to.
	 */
	void optimize_bounding(packing & pack, size_t optimality, bool bitmap, std::string output_file);

	/**
	* Finds a k-optimal placement regarding the wirelength of all nets for the given packing.
	* Writes the solution to the given path when there is one, writes error to console otherwise.
	* @param pack The packing that should be placed.
	* @param optimality k. If zero an optimal placement is searched.
	* @param bitmap Indicates whether to output a bitmap of the best placement.
	* @param output_file The path where to write the output to.
	*/
	void optimize_wirelength(packing & pack, size_t optimality, bool bitmap, std::string output_file);
};

#endif // !INPUT_PARSER_H
