#include "packing.h"

/**
* Determins wether the placement contains a collision (and thus is invalid).
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
std::ostream &operator<<(std::ostream & out, const packing & pack)
{
	for (auto rect : pack.rect_list)
	{
		out << rect << std::endl;
	}

	return out;
}

std::istream &operator>>(std::istream & in, packing & pack)
{
	rectangle rect;
	while (in >> rect)
	{
		rect.id = pack.rect_list.size();
		pack.rect_list.push_back(rect);
	}

	return in;
}