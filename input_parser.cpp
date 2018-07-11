#include "input_parser.h"

std::string input_parser::getOption(char ** begin, char ** end, const std::string & option)
{
	char** it = std::find(begin, end, option);
	if (it != end && ++it != end)
	{
		return std::string(*it);
	}
	return "";
}

bool input_parser::getSwitch(char ** begin, char ** end, const std::string & option)
{
	return std::find(begin, end, option) != end;
}

void input_parser::parse(int argc, char * argv[])
{
	const auto begin = argv;
	const auto end = argv + argc;
	if (getSwitch(begin, end, "--help"))
	{
		print_help();
		return;
	}

	std::cout << "Invalid usage!" << std::endl;
	print_help();
	return;
}

void input_parser::print_help()
{
	std::string help_text = R"(The program usese the following syntax:
input_file [options]

Options are:
--rect: Optimize size of bounding rectangle. 
--wire: Optimize wirelength. Will be ignored if --rect is specified.
--global: Enumerate all possibilites.
--local k: Find a k-optimal solution. Will be ignored if --global is specified.
--bitmap: Write solution to bitmap. 

If no options are supplied, --rect and --global will be used.)";

	std::cout << help_text << std::endl;
}
