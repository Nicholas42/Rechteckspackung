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
	packing pack = read_packing(input_file);

	//We no longer care about the first argument since we assumed it to be filename
	begin++;

	//We assume global (=0) and check for local
	unsigned int optimality = 0;
	std::string optimality_arg = get_option(begin, end, "--local");
	if (!optimality_arg.empty())
	{
		try
		{
			optimality = (size_t)std::stoul(optimality_arg);
			if (optimality >= pack.get_num_rects())
			{
				throw std::out_of_range("optimality");
			}
		}
		catch (const std::invalid_argument&)
		{
			std::cout << optimality_arg << " is not an allowed value!" << std::endl;
			print_help();
			return;
		}
		catch (const std::out_of_range&)
		{
			std::cout << optimality_arg << " is out of range!" << std::endl;
		}
	}

	std::string output_file = get_option(begin, end, "--output");
	if (output_file.empty())
	{
		output_file = input_file + ".out";
	}

	bool bitmap = get_switch(begin, end, "--bitmap");
	if (get_switch(begin, end, "--rect"))
	{
		optimize_bounding(pack, optimality, bitmap, output_file);
		return;
	}
	else
	{
		optimize_wirelength(pack, optimality, bitmap, output_file);
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
--local k: Find a k-optimal solution. k has to be an integer in [0, the number of rectangles). Will be ignored if --global is specified.
--bitmap: Write solution to bitmap. 
--help: Display this text.
--out path: The name and path of the output file. Defaults to input file with ending .out added.

If no options are supplied, --global and --wire will be used.)";

	std::cout << help_text << std::endl;
}

void input_parser::optimize_bounding(packing & pack, size_t optimality, bool bitmap, std::string output_file)
{
	std::cout << "Placing rectangles..." << std::endl;
	packing best_pack;
	pos min_area = std::numeric_limits<pos>::max();

	placement_iterator pl_it(pack, optimality, true);
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

	if (best_pack.get_num_rects() != 0) //Found placement
	{
		std::ofstream outfile(output_file);
		outfile << best_pack;
		outfile.flush();

		std::cout << "Output written to " << output_file << std::endl;

		if (bitmap)
		{
			if (best_pack.init_bmp())
			{
				best_pack.draw_all_rectangles();
				best_pack.write_bmp();
			}
			else
			{
				std::cout << "Instance is too big for a bitmap." << std::endl;
			}
		}
	}
	else //No placement found
	{
		std::cout << "No valid placement was found with the given parameters!" << std::endl;
	}
}

void input_parser::optimize_wirelength(packing & pack, size_t optimality, bool bitmap, std::string output_file)
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
	std::cout << "Value of best packing: " << best_weight << std::endl;

	std::ofstream outfile(output_file);
	outfile << best_pack;
	outfile.flush();

	std::cout << "Output written to " << output_file << std::endl;

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
			std::cout << "Instance is too big for a bitmap." << std::endl;
		}
	}
}

