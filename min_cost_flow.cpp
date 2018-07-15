#include "min_cost_flow.h"


void graph::augment_path(path &p, weight w)
{
    assert(!p.empty());
    assert(w > 0);

    size_t last_node = 0;
    for (auto const &e: p)
    {
        if (_edges.at(e).from != last_node)
        {
            _edges.at(e).flow -= w;
        }
        else
        {
            _edges.at(e).flow += w;
        }

        last_node = other_endpoint(e, last_node);

        assert(_edges.at(e).flow >= 0);
        assert(_edges.at(e).flow <= _edges.at(e).cap);
    }

    assert(w <= _list.at(0).demand);
    assert(w <= -_list.at(last_node).demand);
    _list.at(0).demand -= w;
    _list.at(last_node).demand += w;
}

weight graph::potential_cost(size_t edge_index, bool reverse) const
{
    const edge &e = _edges.at(edge_index);
    weight cost = e.cost + _potential.at(e.from) - _potential.at(e.to);
    if (reverse)
    {
        return -cost;
    }
    else
    {
        return cost;
    }
}

bool graph::compute_min_flow()
{
    if (!compute_starting_potential())
    {
        return false;
    }

    while (_list.at(0).demand > 0)
    {
        assert(!_list.at(0).adjacent.empty());
        path p;
        weight f = compute_shortest_path(p);
        augment_path(p, f);
    }

    return true;
}


// This is O(n^2)-dijkstra. It suffices for the requested runtime and right now I am too lazy for something better.
weight graph::compute_shortest_path(path &ret)
{
    std::vector<weight> distances(_potential.size(), _invalid_cost);
    distances.at(0) = 0;
    //path ret;
    std::vector<size_t> prev_edges(_list.size());
    std::vector<bool> fix(_list.size(), false);

    size_t cur_node = 0;

    while (cur_node != _invalid_index)
    {
        fix.at(cur_node) = true;

        for (const auto &e: _list.at(cur_node).adjacent)
        {
            const edge &cur_edge = _edges.at(e);
            if (!is_allowed(cur_edge, cur_node))
            {
                continue;
            }

            weight new_dist = distances.at(cur_node) + potential_cost(e, cur_edge.from != cur_node);
            size_t neighbour = other_endpoint(e, cur_node);

            if (!fix.at(neighbour) && new_dist < distances.at(neighbour))
            {
                distances.at(neighbour) = new_dist;
                prev_edges.at(neighbour) = e;
            }
        }

        cur_node = _invalid_index;

        for (size_t i = 0; i < distances.size(); ++i)
        {
            if (!fix.at(i) && (cur_node == _invalid_index || distances.at(i) < distances.at(cur_node)))
            {
                cur_node = i;
            }
        }
    }

    size_t t = _invalid_index;

    for (size_t i = 0; i < _list.size(); ++i)
    {
        assert(distances.at(i) < _invalid_cost);
        _potential.at(i) += distances.at(i);
        if (_list.at(i).demand < 0)
        {
            t = i;
        }
    }

    assert(t != _invalid_index);

    size_t cur = t;
    weight max_cap = -_list.at(t).demand;
    while (cur != 0)
    {
        const edge &cur_edge = _edges.at(prev_edges.at(cur));
        ret.push_front(cur_edge.id);
        cur = cur_edge.other_endpoint(cur);
        max_cap = std::min(max_cap, cur_edge.residual_cap(cur));
    }

    max_cap = std::min(max_cap, _list.at(0).demand);

    assert(max_cap > 0);
    return max_cap;
}

bool graph::compute_starting_potential()
{
    _potential.resize(_list.size(), 0);
    bool changed = false;

    for (size_t i = 0; i < _list.size(); ++i)
    {
        changed = false;
        for (auto &cur_edge: _edges)
        {
            if (_potential.at(cur_edge.from) < _invalid_cost &&
                _potential.at(cur_edge.from) + cur_edge.cost < _potential.at(cur_edge.to))
            {
                changed = true;
                _potential.at(cur_edge.to) = _potential.at(cur_edge.from) + cur_edge.cost;
            }
        }

    }

    return !changed;
}

weight graph::place()
{
    weight ret = 0;
    pos base = _pack.get_rect(-1).get_pos(_dim);
    for (auto &n: _list)
    {
        switch (n.type)
        {
            case node_type::source:
                break;
            case node_type::chip_base:
                base = _potential.at(n.index);
                break;
            case node_type::rect_node:
            {
                rectangle &rect = _pack.get_rect(n.object_index);
                rect.base.coord(_dim) = base - _potential.at(n.index);
                rect.base.set = true;
                break;
            }
            case node_type::net_lower_node:
                ret += _potential.at(n.index) * _pack.get_net((size_t) n.object_index).net_weight;
                break;
            case node_type::net_upper_node:
                ret -= _potential.at(n.index) * _pack.get_net((size_t) n.object_index).net_weight;
                break;
        }
    }

    return ret;
}

graph graph::make_graph(packing &pack, dimension dim, sequence_pair sp)
{
    graph ret(pack, dim);

    ret._list.reserve(2 + pack.get_num_rects() + 2 * pack.get_num_nets());

    ret.add_all_nodes();

    for (size_t i = 0; i < pack.get_num_nets(); ++i)
    {
        for (auto p: pack.get_net(i).pin_list)
        {
            ret.add_pin_edges(p, i);
        }
    }

    std::vector<bool> smaller_neg_locus(pack.get_num_rects(), false);

    for (auto it = sp.negative_locus.begin(); it != sp.negative_locus.end(); ++it)
    {
        ret.add_bound_edges(pack.get_rect((int) *it));

        switch (dim)
        {
            case dimension::x:
            {
                ret.add_all_orientations(*it, sp.positive_locus.begin(), sp.positive_locus.end(), smaller_neg_locus);
                break;
            }
            case dimension::y:
            {
                ret.add_all_orientations(*it, sp.positive_locus.rbegin(), sp.positive_locus.rend(), smaller_neg_locus);
                break;
            }
        }

        smaller_neg_locus.at(*it) = true;
    }

    return ret;
}

void graph::add_arc(size_t from, size_t to, weight cost, weight cap)
{
    node &from_node = _list.at(from);
    node &to_node = _list.at(to);

    from_node.adjacent.push_back(_edges.size());
    to_node.adjacent.push_back(_edges.size());

    _edges.emplace_back(from, to, _edges.size(), cost, cap);
}

size_t graph::get_node_index(node_type type, size_t index) const
{
    switch (type)
    {
        case node_type::source:
            return 0;
        case node_type::chip_base:
            return 1;
        case node_type::rect_node:
            return 2 + index;
        case node_type::net_lower_node:
            return 2 + _pack.get_num_rects() + 2 * index;
        case node_type::net_upper_node:
            return 2 + _pack.get_num_rects() + 2 * index + 1;
        default:
            throw std::invalid_argument("Invalid argument: Unspecified value for type");
    }
}

void graph::add_bound_edges(const rectangle &rect)
{
    size_t index = get_node_index(node_type::rect_node, (size_t) rect.id);
    size_t chip_base = get_node_index(node_type::chip_base);
    add_arc(chip_base, index, _pack.get_chip_base().get_pos(_dim));
    add_arc(index, chip_base, rect.get_dimension(_dim) - _pack.get_chip_base().get_max(_dim));
}

void graph::add_pin_edges(const pin &p, size_t net_id)
{
    pos rel_pin_pos = _pack.get_rect(p.index).get_relative_pin_position(p, _dim);
    size_t pin_index = get_node_index(node_type::rect_node, (size_t) p.index);
    add_arc(get_node_index(node_type::net_lower_node, net_id), pin_index, -rel_pin_pos);
    add_arc(pin_index, get_node_index(node_type::net_upper_node, net_id), rel_pin_pos);
}

void graph::add_orientation_edges(size_t smaller, size_t bigger)
{
    add_arc(get_node_index(node_type::rect_node, smaller), get_node_index(node_type::rect_node, bigger),
            _pack.get_rect((int) smaller).get_dimension(_dim));
}

std::ostream &operator<<(std::ostream &out, const graph &g)
{
    weight flow_value = 0;
    for (auto n: g._list)
    {
        out << "Index: " << n.index << "; Demand: " << n.demand << "; Type" << (int) n.type << std::endl;
        for (auto edge_id: n.adjacent)
        {
            const edge &e = g._edges.at(edge_id);
            flow_value += e.flow * e.cost;
            out << "\t";
            if (e.from != n.index)
            {
                out << "R; From: ";
            }
            else
            {
                out << "To: ";
            }

            out << g.other_endpoint(edge_id, n.index) << "; Cost:" << e.cost << "; Flow: " << e.flow << std::endl;
        }
    }

    out << "Flow value: " << flow_value << std::endl;

    return out;
}

void graph::to_dot(std::string outfile, bool flow)
{
    std::ofstream file(outfile);
    file << "digraph dings { " << std::endl;

    for (auto n : _list)
    {
        file << n.index << " [label=" << n.index << ", color=" << type_to_color[(int) n.type] << "];" << std::endl;
        for (auto edge_id : n.adjacent)
        {
            const edge &e = _edges.at(edge_id);
            if (e.from == n.index && (!flow || e.flow > 0))
            {
                file << e.from << " -> " << e.to << " [label=\"" << e.cost << "," << e.flow << "\"];" << std::endl;
            }
        }
    }
    file << "}" << std::endl;
}

void graph::add_node(node &&n)
{
    _list.push_back(n);

    assert(_list.back().index == _list.size() - 1);

    if (_list.size() > 1)
    {
        if (n.demand > 0)
        {
            add_arc(0, _list.size() - 1, 0, _list.back().demand);
            _list.at(0).demand += _list.back().demand;
            _list.back().demand = 0;
        }
    }
}

size_t graph::other_endpoint(size_t edge_index, size_t first_node) const
{
    return _edges.at(edge_index).other_endpoint(first_node);
}

bool graph::is_allowed(const edge &e, size_t from) const
{
    if (e.from != from)
    {
        return e.flow > 0;
    }
    else
    {
        return e.flow < e.cap;
    }
}

void graph::add_all_nodes()
{
    add_node(node(0, 0, node_type::source));
    add_node(node(1, 0, node_type::chip_base));

    for (size_t i = 0; i < _pack.get_num_rects(); ++i)
    {
        add_node(node(get_node_index(node_type::rect_node, i), (int) i, node_type::rect_node));
    }

    for (size_t i = 0; i < _pack.get_num_nets(); ++i)
    {
        add_node(node(_pack.get_net(i), get_node_index(node_type::net_lower_node, i), true));
        add_node(node(_pack.get_net(i), get_node_index(node_type::net_upper_node, i), false));
    }
}

template<class Iterator>
void graph::add_all_orientations(size_t rect_index, const Iterator &begin, const Iterator &end,
                                 const std::vector<bool> &smaller_negative_locus)
{
    for (auto it = begin; it != end; ++it)
    {
        if (*it == rect_index)
        {
            return;
        }
        if (smaller_negative_locus.at(*it))
        {
            add_orientation_edges(rect_index, *it);
        }
    }

    throw std::runtime_error(
            "The rectangle " + std::to_string(rect_index) + " is missing from the postive locus of the sequence pair.");
}

size_t edge::other_endpoint(size_t first) const
{
    assert(first == from || first == to);
    return first != from ? from : to;
}

weight edge::residual_cap(size_t from_) const
{
    if (from_ == from)
    {
        return cap == _invalid_cost ? _invalid_cost : cap - flow;
    }
    else
    {
        return flow;
    }
}

