#include "packing.h"

/**
 * Compares two pointers to rectangles by comparing the rectangles to which they point.
 */
bool rect_ptr_compare::operator() (rectangle *r, rectangle *l) const
{
	return (*r) < (*l);
}

bool rect_ind_compare::operator()(size_t first, size_t second) const
{
    return elements[first] < elements[second];
}

/**
* Determines wether the placement contains a collision (and thus is invalid).
* Gives the indices of two colliding rectangles as an certificate or (-1, -1)
* if there is no collision.
*/
sequence_pair packing::to_sequence_pair() const
{
    std::list<size_t> up_list;
    std::list<size_t> down_list;
    std::vector<std::list<size_t>::iterator> up_pos(rect_list.size(), up_list.end());
    std::vector<std::list<size_t>::iterator> down_pos(rect_list.size(), down_list.end());

    std::vector<size_t> indices(rect_list.size());
    std::iota(indices.begin(), indices.end(), 0);

    // We want to sweep over the rectangles from left to right
    std::sort(indices.begin(), indices.end(), [this](size_t first, size_t second)
    {
        return rectangle::compare(rect_list[first], rect_list[second]);
    });

    // The sweeping line is always ordered from bottom to top
    //TODO compare
    std::set<size_t, rect_ind_compare> line(rect_ind_compare{rect_list});

    for (auto &i : indices)
    {
        const rectangle &rec = rect_list[i];

        auto in = line.insert(i);
        assert(in.second);

        auto it = in.first;

        // First we want to find the next rectangle below which is still active
        if (it != line.begin() && rect_list[*std::prev(it)].contains_y(rec.y))
        {
            // rec is before the previous in the sp
            if (rect_list[*std::prev(it)].contains_x(rec.x))
            {
                throw std::runtime_error("Packing is invalid, impossible to find sequence pair");
            }
            // TODO: insert before.
            up_pos[i] = up_list.insert(up_pos[*std::prev(it)], i);
            line.erase(std::prev(it));
        } else
        {
            //TODO insert after
            if (it == line.begin())
            {
                up_list.push_front(i);
                up_pos[i] = up_list.begin();
            } else
            {
                up_pos[i] = up_list.insert(std::next(up_pos[*std::prev(it)]), i);
            }
        }

        it++;
        // Since this does not intersect our rectangle we can go on to the rectangles above
        while (it != line.end() && rec.contains_y(rect_list[*it].y))
        {
            // This looks pretty different than the other direction, due to
            // a) line.end() points behind the end of line
            // b) line.erase returns an iterator pointing on the element behind the deleted one

            if (rect_list[*it].contains_x(rec.x))
            {
                throw std::runtime_error("Packing is invalid, impossible to find sequence pair");
            }
            it = line.erase(it);
        }

        if (it == line.end())
        {
            down_pos[i] = down_list.insert(down_list.end(), i);
        } else
        {
            down_pos[i] = down_list.insert(down_pos[*it], i);
        }
        // So this rectangle does not intersect with one already there, therefore, we can go on
    }
    return std::make_pair(up_list, down_list);
}

/**
* Determines wether the placement contains a collision (and thus is invalid).
* Gives the indices of two colliding rectangles as an certificate or (-1, -1) 
* if there is no collision.
*/
std::pair<int, int> packing::is_valid() const
{
	std::vector<rectangle> rect_list_cpy(rect_list);

	// We want to sweep over the rectangles from left to right
	std::sort(rect_list_cpy.begin(), rect_list_cpy.end(), rectangle::compare);

	// The sweeping line is always ordered from bottom to top
	sweepline line;

	for (auto &rec : rect_list_cpy)
	{
		std::pair<sweepline::iterator, bool> in = line.insert(&rec);
		assert(in.second);

		sweepline::iterator it = in.first;

		// First we want to find the next rectangle below which is still active
		while (it != line.begin() && !(*std::prev(it))->contains_x(rec.x))
		{
			line.erase(std::prev(it));
		}

		if (it != line.begin() && rec.intersects(**std::prev(it)))
		{
			return {rec.id, (*std::prev(it))->id};
		}

		it++;
		// Since this does not intersect our rectangle we can go on to the rectangles above
		while (it != line.end() && !(*it)->contains_x(rec.x))
		{
			// This looks pretty different than the other direction, due to
			// a) line.end() points behind the end of line
			// b) line.erase returns an iterator pointing on the element behind the deleted one
			it = line.erase(it);
		}

		if (it != line.end() && (*it)->intersects(rec))
		{
			return {rec.id, (*it)->id};
		}
		// So this rectangle does not intersect with one already there, therefore, we can go on
	}

	return {-1, -1};
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
void packing::read_sol_from(const std::string filename)
{
	std::ifstream file(filename);

	if(!file)
	{
		throw std::runtime_error("File not " + filename + " not found.");
	}

	rect_list.clear();

	rectangle rect;
	while (file >> rect)
	{
		rect.id = rect_list.size();
		rect_list.push_back(rect);
	}
}

/**
 *  Reads just the dimension from the instance file.
 */
void packing::read_dimension_from_inst(const std::string filename)
{
	base_filename = filename;
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
}

/**
 * Read an instance, i.e. size of the chip, a list of unplaced rectangles and blockages,
 * and a list of nets.
 */
void packing::read_inst_from(const std::string filename)
{
	read_dimension_from_inst(filename);

	std::ifstream file(filename);

	rectangle rect;
	while (file >> rect)
	{
		rect.id = rect_list.size();
		rect_list.push_back(rect);
	}

	file.clear();

	net n;
	while(file >> n)
	{
		net_list.push_back(n);
	}
}

void packing::draw_all_rectangles()
{
	assert(bmp.initialized);

	for(auto r : rect_list)
	{
		bmp.draw_rectangle(r.x, r.x_max(), r.y, r.y_max(), BLACK);
		bmp.fill_rectangle(r.x, r.x_max(), r.y, r.y_max(), GREEN);
	}
}

void packing::draw_all_pins()
{
	assert(bmp.initialized);

	for(auto n : net_list)
	{
		for(auto p : n.pin_list)
		{
			pos x,y;
			if(p.index == -1)
			{
				x = p.x;
				y = p.y;
			}
			else
			{
				std::tie(x,y) = rect_list[p.index].get_absolute_pin_position(p);
			}
			bmp.draw_point(x, y, BLUE);
		}
	}
}

void packing::draw_cert(const std::pair<int, int> cert)
{
	if(cert.first > -1 && cert.second > -1)
	{
		const rectangle r = rect_list[cert.first].intersection(rect_list[cert.second]);
		bmp.draw_rectangle(r.x, r.x_max(), r.y, r.y_max(), BLACK);
		bmp.fill_rectangle(r.x, r.x_max(), r.y, r.y_max(), RED);
	}
}

void packing::write_bmp()
{
	assert(bmp.initialized);
	bmp.write();
	std::cout << "Wrote bitmap to " << bmp.filename << std::endl;
}

bool packing::init_bmp()
{
	const int width = x_max - x_min;
	const int height = y_max - y_min;
	const int scaling = std::max(std::min(1000/width, 1000/height), 1);
	if(bitmap::valid(width * scaling, height * scaling))
	{
		bmp = bitmap(base_filename + ".bmp", width, height, scaling);
	}

	return bmp.initialized;
}