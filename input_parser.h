#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <algorithm>
#include <string>
#include <iostream>

class input_parser
{
private:
	std::string getOption(char** begin, char** end, const std::string & option);
	bool getSwitch(char** begin, char** end, const std::string & option);
public:
	void parse(int argc, char* argv[]);
	void print_help();
};

#endif // !INPUT_PARSER_H
