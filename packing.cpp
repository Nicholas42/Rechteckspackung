#include "packing.h"

/**
* Determines wether the placement contains a collision (and thus is invalid).
* true means that no collision was found
*/
bool packing::is_valid()
{
	// We want to sweep over the rectangles from left to right
	std::sort(rect_list.begin(), rect_list.end(), rectangle::compare);

	// The sweeping line is always ordered from bottom to top
	// TODO: maybe just pointer on the rectangles? Now, we copy each rectangle once
	std::set<rectangle> line;

	for (auto rec : rect_list)
	{
		std::pair<std::set<rectangle>::iterator, bool> in = line.insert(rec);
		assert(in.second);

		std::set<rectangle>::iterator it = in.first;

		// First we want to find the next rectangle below which is still active
		while (it != line.begin() && !std::prev(it)->contains_x(rec.x))
		{
			line.erase(std::prev(it));
		}

		if (it != line.begin() && rec.intersects(*std::prev(it)))
		{
			return false;
		}

		it++;
		// Since this does not intersect our rectangle we can go on to the rectangles above
		while (it != line.end() && !it->contains_x(rec.x))
		{
			// This looks pretty different than the other direction, due to
			// a) line.end() points behind the end of line
			// b) line.erase returns an iterator pointing on the element behind the deleted one
			it = line.erase(it);
		}

		if (it != line.end() && it->intersects(rec))
		{
			return false;
		}
		// So this rectangle does not intersect with one already there, therefore, we can go on
	}

	return true;
}

//TODO: Very naive dummy implementation so far
// Seems fine to me. This is literally what we shall do.
std::ostream &operator<<(std::ostream & out, const packing & pack)
{
	for (auto rect : pack.rect_list)
	{
		assert(pack.x_min <= rect.x && pack.x_max >= rect.x_max() &&
			   pack.y_min <= rect.y && pack.y_max >= rect.y_max());

		out << rect << std::endl;
	}

	return out;
}

/**
 * Reads solution, i.e. a list of placed rectangles.
 */
void packing::read_sol_from(std::string filename)
{
	std::ifstream file(filename);

	if(!file)
	{
		throw std::runtime_error("File not " + filename + " not found.");
	}

	rectangle rect;
	while (file >> rect)
	{
		rect.id = rect_list.size();
		rect_list.push_back(rect);
	}
}

/**
 * Read an instance, i.e. size of the chip, a list of unplaced rectangles and blockages,
 * and a list of nets.
 */
void packing::read_inst_from(std::string filename)
{
	std::ifstream file(filename);

	if(!file)
	{
		throw std::runtime_error("File not " + filename + " not found.");
	}

	if(!(file >> x_min >> x_max >> y_min >> y_max))
	{
		throw std::runtime_error("Invalid Format in " + filename);
	}

	if(x_max < x_min || y_max < y_min)
	{
		throw std::runtime_error("Invalid Format in " + filename);
	}

	rectangle rect;
	while (file >> rect)
	{
		rect.id = rect_list.size();
		rect_list.push_back(rect);
	}

	net n;
	while(file >> n)
	{
		net_list.push_back(n);
	}
}