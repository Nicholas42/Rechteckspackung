#include "packing.h"

/**
 * Compares two pointers to rectangles by comparing the rectangles to which they point.
 */
bool rect_ptr_compare::operator()(rectangle *r, rectangle *l) const
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
    sequence_pair seq_pair;
    std::vector<std::list<size_t>::iterator> up_pos(rect_list.size(), seq_pair.positive_locus.end());
    std::vector<std::list<size_t>::iterator> down_pos(rect_list.size(), seq_pair.negative_locus.end());

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
        if (it != line.begin() && rect_list[*std::prev(it)].contains_y(rec.base.y))
        {
            // rec is before the previous in the sp
            if (rect_list[*std::prev(it)].contains_x(rec.base.x))
            {
                throw std::runtime_error("Packing is invalid, impossible to find sequence pair");
            }
            // TODO: insert before.
            up_pos[i] = seq_pair.positive_locus.insert(up_pos[*std::prev(it)], i);
            line.erase(std::prev(it));
        }
        else
        {
            //TODO insert after
            if (it == line.begin())
            {
                seq_pair.positive_locus.push_front(i);
                up_pos[i] = seq_pair.positive_locus.begin();
            }
            else
            {
                up_pos[i] = seq_pair.positive_locus.insert(std::next(up_pos[*std::prev(it)]), i);
            }
        }

        it++;
        // Since this does not intersect our rectangle we can go on to the rectangles above
        while (it != line.end() && rec.contains_y(rect_list[*it].base.y))
        {
            // This looks pretty different than the other direction, due to
            // a) line.end() points behind the end of line
            // b) line.erase returns an iterator pointing on the element behind the deleted one

            if (rect_list[*it].contains_x(rec.base.x))
            {
                throw std::runtime_error("Packing is invalid, impossible to find sequence pair");
            }
            it = line.erase(it);
        }

        if (it == line.end())
        {
            down_pos[i] = seq_pair.negative_locus.insert(seq_pair.negative_locus.end(), i);
        }
        else
        {
            down_pos[i] = seq_pair.negative_locus.insert(down_pos[*it], i);
        }
        // So this rectangle does not intersect with one already there, therefore, we can go on
    }
    return seq_pair;
}

/**
* Determines wether the placement contains a collision (and thus is invalid).
* Gives the indices of two colliding rectangles as an certificate or (-1, -1) 
* if there is no collision.
*/
const certificate packing::is_valid() const
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
        while (it != line.begin() && !(*std::prev(it))->contains_x(rec.base.x))
        {
            line.erase(std::prev(it));
        }

        if (it != line.begin() && rec.intersects(**std::prev(it)))
        {
            return certificate(rec.id, (*std::prev(it))->id);
        }

        it++;
        // Since this does not intersect our rectangle we can go on to the rectangles above
        while (it != line.end() && !(*it)->contains_x(rec.base.x))
        {
            // This looks pretty different than the other direction, due to
            // a) line.end() points behind the end of line
            // b) line.erase returns an iterator pointing on the element behind the deleted one
            it = line.erase(it);
        }

        if (it != line.end() && (*it)->intersects(rec))
        {
            return certificate(rec.id, (*it)->id);
        }
        // So this rectangle does not intersect with one already there, therefore, we can go on
    }

    return certificate();
}

//TODO: Very naive dummy implementation so far
// Seems fine to me. This is literally what we shall do.
std::ostream &operator<<(std::ostream &out, const packing &pack)
{
    out << "#Nets: " << pack.get_num_nets() << "; #Rects " << pack.get_num_rects() << std::endl;
    for (auto &rect : pack.rect_list)
    {
        assert(pack.chip_base.get_pos(dimension::x) <= rect.get_pos(dimension::x));
        assert(pack.chip_base.get_max(dimension::x) >= rect.get_max(dimension::x));
        assert(pack.chip_base.get_pos(dimension::y) <= rect.get_max(dimension::y));
        assert(pack.chip_base.get_max(dimension::y) >= rect.get_max(dimension::y));

        out << rect << std::endl;
    }
    for (auto &n : pack.net_list)
    {
        for (auto &p : n.pin_list)
        {
            out << pack.get_rect(p.index).get_absolute_pin_position(p) << std::endl;
        }
    }

    return out;
}

/**
 * Reads solution, i.e. a list of placed rectangles.
 */
void packing::read_sol_from(const std::string filename)
{
    std::ifstream file(filename);

    if (!file)
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

    if (!file)
    {
        throw std::runtime_error("File not " + filename + " not found.");
    }

    file >> chip_base;
}

/**
 * Read an instance, i.e. size of the chip, a list of unplaced rectangles and blockages,
 * and a list of nets.
 */
void packing::read_inst_from(const std::string filename)
{
    std::ifstream file(filename);
    base_filename = filename;

    if (!(file >> chip_base))
    {
        throw std::runtime_error("File " + filename + " has invalid format.");
    }

    rectangle rect;
    while (file >> rect)
    {
        if (!rect.blockage)
        {
            rect.id = rect_list.size();
            rect_list.push_back(rect);
        }
    }

    file.clear();

    net n;
    while (file >> n)
    {
        n.index = net_list.size();
        net_list.push_back(n);
    }
}

void packing::draw_all_rectangles()
{
    assert(bmp.initialized);

    for (auto r : rect_list)
    {
        bmp.draw_rectangle(r, BLACK);
        bmp.fill_rectangle(r, GREEN);
    }
}

void packing::draw_all_pins()
{
    assert(bmp.initialized);

    for (auto n : net_list)
    {
        for (auto p : n.pin_list)
        {
            point pin_point = get_rect(p.index).get_absolute_pin_position(p);
            bmp.draw_point(pin_point, BLUE);
        }
    }
}

void packing::draw_cert(const certificate &cert)
{
    if (!cert.valid)
    {
        const rectangle r = rect_list[cert.first].intersection(rect_list[cert.second]);
        bmp.draw_rectangle(r, BLACK);
        bmp.fill_rectangle(r, RED);
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
    const int width = chip_base.get_dimension(dimension::x);
    const int height = chip_base.get_dimension(dimension::y);
    const int scaling = std::max(std::min(1000 / width, 1000 / height), 1);
    if (bitmap::valid(width * scaling, height * scaling))
    {
        bmp = bitmap(base_filename + ".bmp", width, height, scaling);
    }

    return bmp.initialized;
}

//TODO: maybe rename
const rectangle &packing::get_rect(int index) const
{
    if (index < 0)
    {
        return chip_base;
    }
    else
    {
        return rect_list.at((size_t) index);
    }
}

size_t packing::get_num_rects() const
{
    return rect_list.size();
}

size_t packing::get_num_nets() const
{
    return net_list.size();
}

void packing::move_rect(int index, point pos)
{
	if (index < 0)
	{
		throw new std::out_of_range("index");
	}
	else
	{
		rectangle &rect = get_rect(index);
		rect.base = pos;
		rect.base.set = true;
	}
}

const net &packing::get_net(size_t index) const
{
    return net_list.at(index);
}

const rectangle &packing::get_chip_base() const
{
    return chip_base;
}

rectangle &packing::get_rect(int index)
{
    if (index < 0)
    {
        return chip_base;
    }
    else
    {
        return rect_list.at((size_t) index);
    }
}

weight packing::compute_netlength() const
{
    weight ret = 0;
    for (auto &n: net_list)
    {
        bounding_box b;
        for (auto &p: n.pin_list)
        {
            b.add_point(get_rect(p.index).get_absolute_pin_position(p));
        }

        ret += n.net_weight * ((weight) b.half_circumference());
    }

    return ret;
}

void packing::draw_all_nets()
{
    for (auto const &n : net_list)
    {
        bounding_box b;
        for (auto const &p : n.pin_list)
        {
            b.add_point(get_rect(p.index).get_absolute_pin_position(p));
        }
        bmp.draw_rectangle(b.to_rectangle(), RED);
    }
}

weight packing::compute_netlength_optimal(const sequence_pair &list)
{
    weight value = 0;
    for (auto dim: all_dimensions)
    {
        graph g = graph::make_graph(*this, dim, list);
        if (!g.compute_min_flow())
        {
            return _invalid_cost;
        }
        value += g.place(*this);
    }

    assert(value == compute_netlength());

    return value;
}

rectangle_iterator packing::get_iter()
{
    return rectangle_iterator(rect_list);
}

pos bounding_box::half_circumference() const
{
    assert(max.set && min.set);
    pos sum = 0;
    for (auto dim: all_dimensions)
    {
        sum += max.coord(dim) - min.coord(dim);
    }
    return sum;
}

void bounding_box::add_point(const point &p)
{
    if (max.set)
    {
        for (auto dim: all_dimensions)
        {
            max.coord(dim) = std::max(p.coord(dim), max.coord(dim));
            min.coord(dim) = std::min(p.coord(dim), min.coord(dim));
        }
    }
    else
    {
        max = p;
        min = p;
        max.set = true;
        min.set = true;
    }
}

rectangle bounding_box::to_rectangle() const
{
    return {min, max};
}

rectangle_iterator &rectangle_iterator::operator++()
{
    _at_end = true;
    for (auto &rect : _rect_list)
    {
        rect.rotate(rotation::rotated_90);
        if (rect.rot != rotation::rotated_0)
        {
            _at_end = false;
            break;
        }

        rect.flip();

        if (rect.flipped)
        {
            _at_end = false;
            break;
        }
    }
    return *this;
}

rectangle_iterator::operator bool() const
{
    return !_at_end;
}
