#include "input_parser.h"

std::string input_parser::get_option(char ** begin, char ** end, const std::string & option)
{
	char** it = std::find(begin, end, option);
	if (it != end && ++it != end)
	{
		return std::string(*it);
	}
	return "";
}

bool input_parser::get_switch(char ** begin, char ** end, const std::string & option)
{
	return std::find(begin, end, option) != end;
}

packing input_parser::read_packing(std::string filename)
{
	packing pack;
	pack.read_inst_from(filename);
	return pack;
}

void input_parser::parse(int argc, char * argv[])
{
	//We don't care about the application name
	auto begin = argv + 1;
	const auto end = argv + argc;
	bool invalid = false;
	if (get_switch(begin, end, "--help"))
	{
		print_help();
		return;
	}

	if (argc < 2)
	{
		std::cout << "Missing arguments!" << std::endl;
		print_help();
		return;
	}

	std::string input_file(argv[1]);
	//We no longer care about the first argument since we assumed it to be filename
	begin++;

	//We assume global (=0) and check for local
	unsigned int optimality = 0;
	std::string optimality_arg = get_option(begin, end, "--local");
	if (!optimality_arg.empty())
	{
		try
		{
			optimality = std::stoi(optimality_arg);
		}
		catch (const std::exception&)
		{
			std::cout << optimality_arg << " is not an allowed value!" << std::endl;
			print_help();
			return;
		}
	}

	bool bitmap = get_switch(begin, end, "--bitmap");
	packing pack = read_packing(input_file);
	if (get_switch(begin, end, "--rect"))
	{
		optimize_bounding(pack, optimality, bitmap);
		return;
	}
	else
	{
		optimize_wirelength(pack, optimality, bitmap);
		return;
	}
}

void input_parser::print_help()
{
	std::string help_text = R"(The program usese the following syntax:
input_file [options]

Options are:
--rect: Optimize size of bounding rectangle. 
--wire: Optimize wirelength. Will be ignored if --rect is specified.
--global: Enumerate all possibilites.
--local k: Find a k-optimal solution. k has to be an integer between 1 and the number of rectangles. Will be ignored if --global is specified.
--bitmap: Write solution to bitmap. 
--help: Display this text.

If no options are supplied, --rect and --wire will be used.)";

	std::cout << help_text << std::endl;
}

void input_parser::optimize_bounding(packing & pack, int optimality, bool bitmap)
{
	std::cout << "Placing rectangles..." << std::endl;
	packing best_pack;
	pos min_area = std::numeric_limits<pos>::max();

	placement_iterator pl_it(pack, optimality, true);
	size_t cnt = 1;
	do
	{
		if ((*pl_it).apply_to(pack))
		{
			pos cur_area = pack.calculate_area();
			if (cur_area < min_area)
			{
				best_pack = pack;
				min_area = cur_area;
			}
		}
	} while (++pl_it);

	std::cout << best_pack.calculate_area() << std::endl;
	if (bitmap && best_pack.init_bmp())
	{
		best_pack.draw_all_rectangles();
		best_pack.write_bmp();
	}
}

void input_parser::optimize_wirelength(packing & pack, int optimality, bool bitmap)
{
	packing best_pack;
	weight best_weight = _invalid_cost;
	placement_iterator pl_it(pack, optimality, false);

	while (pl_it)
	{
		weight new_weight = pack.compute_netlength_optimal(*pl_it);
		if (new_weight < best_weight)
		{
			std::cout << *pl_it;
			best_pack = pack;
			best_weight = new_weight;
		}
		++pl_it;
	}
	std::cout << "Value: " << best_weight << std::endl;
	std::cout << best_pack;
	std::cout << best_pack.to_sequence_pair();

	if (bitmap)
	{
		if (best_pack.init_bmp())
		{
			best_pack.draw_all_rectangles();
			best_pack.draw_all_nets();
			best_pack.draw_all_pins();
			best_pack.write_bmp();
		}
		else
		{
			std::cout << "Instance is to big for a bitmap." << std::endl;
		}
	}
}

