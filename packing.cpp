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

sequence_pair packing::to_sequence_pair() const
{
    sequence_pair seq_pair;
    std::vector<std::list<size_t>::iterator> up_pos(_rect_list.size(), seq_pair.positive_locus.end());
    std::vector<std::list<size_t>::iterator> down_pos(_rect_list.size(), seq_pair.negative_locus.end());

    std::vector<size_t> indices(_rect_list.size());
    std::iota(indices.begin(), indices.end(), 0);

    // We want to sweep over the rectangles from left to right
    std::sort(indices.begin(), indices.end(), [this](size_t first, size_t second)
    {
        return rectangle::compare(_rect_list[first], _rect_list[second]);
    });

    // The sweeping line is always ordered from bottom to top
    std::set<size_t, rect_ind_compare> line(rect_ind_compare{_rect_list});

    for (auto &i : indices)
    {
        const rectangle &rec = _rect_list[i];

        auto in = line.insert(i);
        assert(in.second);

        auto it = in.first;

        // First we want to find the next rectangle below which is still active
        if (it != line.begin() && _rect_list[*std::prev(it)].contains_y(rec.base.y))
        {
            // rec is before the previous in the sp
            if (_rect_list[*std::prev(it)].contains_x(rec.base.x))
            {
                throw std::runtime_error("Packing is invalid, impossible to find sequence pair");
            }
            up_pos[i] = seq_pair.positive_locus.insert(up_pos[*std::prev(it)], i);
            line.erase(std::prev(it));
        }
        else
        {
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
        while (it != line.end() && rec.contains_y(_rect_list[*it].base.y))
        {
            // This looks pretty different than the other direction, due to
            // a) line.end() points behind the end of line
            // b) line.erase returns an iterator pointing on the element behind the deleted one

            if (_rect_list[*it].contains_x(rec.base.x))
            {
                throw std::runtime_error("Packing is invalid, impossible to find sequence pair");
            }
            it = line.erase(it);
        }

        if (it == line.end())
        {
            down_pos[i] = seq_pair.negative_locus.insert(seq_pair.negative_locus.begin(), i);
        }
        else
        {
            down_pos[i] = seq_pair.negative_locus.insert(std::next(down_pos[*it]), i);
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
    std::vector<rectangle> rect_list_cpy(_rect_list);

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
    for (auto &rect : pack._rect_list)
    {
        assert(pack._chip_base.get_pos(dimension::x) <= rect.get_pos(dimension::x));
        assert(pack._chip_base.get_max(dimension::x) >= rect.get_max(dimension::x));
        assert(pack._chip_base.get_pos(dimension::y) <= rect.get_max(dimension::y));
        assert(pack._chip_base.get_max(dimension::y) >= rect.get_max(dimension::y));

        out << rect << std::endl;
    }
    for (auto &n : pack._net_list)
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

    _rect_list.clear();

    rectangle rect;
    while (file >> rect)
    {
        rect.id = _rect_list.size();
        _rect_list.push_back(rect);
    }
}

/**
 *  Reads just the dimension from the instance file.
 */
void packing::read_dimension_from_inst(const std::string filename)
{
    _base_filename = filename;
    std::ifstream file(filename);

    if (!file)
    {
        throw std::runtime_error("File not " + filename + " not found.");
    }

    file >> _chip_base;
}

/**
 * Read an instance, i.e. size of the chip, a list of unplaced rectangles and blockages,
 * and a list of nets.
 */
void packing::read_inst_from(const std::string filename)
{
    std::ifstream file(filename);
    _base_filename = filename;

    if (!(file >> _chip_base))
    {
        throw std::runtime_error("File " + filename + " has invalid format.");
    }

    rectangle rect;
    while (file >> rect)
    {
        if (!rect.blockage)
        {
            rect.id = _rect_list.size();
            _rect_list.push_back(rect);
        }
    }

    file.clear();

    net n;
    while (file >> n)
    {
        n.index = _net_list.size();
        _net_list.push_back(n);
    }
}

void packing::draw_all_rectangles()
{
    assert(_bmp.initialized);

    for (auto r : _rect_list)
    {
        _bmp.draw_rectangle(r, BLACK);
        _bmp.fill_rectangle(r, GREEN);
    }
}

void packing::draw_all_pins()
{
    assert(_bmp.initialized);

    for (auto n : _net_list)
    {
        for (auto p : n.pin_list)
        {
            point pin_point = get_rect(p.index).get_absolute_pin_position(p);
            _bmp.draw_point(pin_point, BLUE);
        }
    }
}

void packing::draw_cert(const certificate &cert)
{
    if (!cert.valid)
    {
        const rectangle r = _rect_list[cert.first].intersection(_rect_list[cert.second]);
        _bmp.draw_rectangle(r, BLACK);
        _bmp.fill_rectangle(r, RED);
    }
}

void packing::write_bmp()
{
    assert(_bmp.initialized);
    _bmp.write();
    std::cout << "Wrote bitmap to " << _bmp.filename << std::endl;
}

bool packing::init_bmp()
{
    const int width = _chip_base.get_dimension(dimension::x);
    const int height = _chip_base.get_dimension(dimension::y);
    const int scaling = std::max(std::min(1000 / width, 1000 / height), 1);
    if (bitmap::valid(width * scaling, height * scaling))
    {
        _bmp = bitmap(_base_filename + ".bmp", width, height, scaling);
    }

    return _bmp.initialized;
}

//TODO: maybe rename
const rectangle &packing::get_rect(int index) const
{
    if (index < 0)
    {
        return _chip_base;
    }
    else
    {
        return _rect_list.at((size_t) index);
    }
}

size_t packing::get_num_rects() const
{
    return _rect_list.size();
}

size_t packing::get_num_nets() const
{
    return _net_list.size();
}

void packing::move_rect(int index, point pos)
{
    if (index < 0)
    {
        throw std::out_of_range("index");
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
    return _net_list.at(index);
}

const rectangle &packing::get_chip_base() const
{
    return _chip_base;
}

rectangle &packing::get_rect(int index)
{
    if (index < 0)
    {
        return _chip_base;
    }
    else
    {
        return _rect_list.at((size_t) index);
    }
}

weight packing::compute_netlength() const
{
    weight ret = 0;
    for (auto &n: _net_list)
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
    for (auto const &n : _net_list)
    {
        bounding_box b;
        for (auto const &p : n.pin_list)
        {
            b.add_point(get_rect(p.index).get_absolute_pin_position(p));
        }
        _bmp.draw_rectangle(b.to_rectangle(), BLUE);
    }
}

weight packing::compute_netlength_optimal(const sequence_pair &sp)
{
    weight value = 0;
    for (auto dim: all_dimensions)
    {
        graph g = graph::make_graph(*this, dim, sp);
        if (!g.compute_min_flow())
        {
            return _invalid_cost;
        }
        value += g.place();
    }

    assert(value == compute_netlength());

    return value;
}

pos packing::calculate_area()
{
    if (_rect_list.size() == 0)
    {
        return 0;
    }

    pos area = 1;
    for (auto dim : all_dimensions)
    {
        area *= std::max_element(_rect_list.begin(), _rect_list.end(), [dim](rectangle rect1, rectangle rect2)
        { return rect1.get_max(dim) < rect2.get_max(dim); })->get_max(dim);
    }

    return area;
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
