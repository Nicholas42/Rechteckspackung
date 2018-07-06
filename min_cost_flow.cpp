#include "min_cost_flow.h"


void graph::augpath(path &p, weight w)
{
    assert(!p.empty());

    if (w == 0)
    {
        w = _list.at(0).demand;

        size_t last_node = 0;

        for (auto edge_it : p)
        {
            if (_edges.at(edge_it).from != last_node)
            {
                w = std::min(w, _edges.at(edge_it).flow);
            }
            else
            {
                assert(_edges.at(edge_it).cap - _edges.at(edge_it).flow > 0);
                w = std::min(w, _edges.at(edge_it).cap - _edges.at(edge_it).flow);
            }

            assert(w > 0);
            last_node = other_endpoint(edge_it, last_node);
        }

        w = std::min(w, -_list.at(last_node).demand);
        //std::cout << "; To: " << last_node;
    }

    assert(w > 0);

    //std::cout << " by " << w << std::endl;

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

weight graph::potential_cost(size_t edge_id, bool reverse) const
{
    const edge &e = _edges.at(edge_id);
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
    std::cout << "Computing starting potential." << std::endl;
    if(compute_starting_potential())
    {
        return false;
    }
    /*
    std::cout << "Potential:" << std::endl;
    for (size_t i = 0; i < _potential.size(); ++i)
    {
        std::cout << i << ": " << _potential.at(i) << std::endl;
    }
    */
    std::cout << "Starting succesive shortest paths." << std::endl;

    while (_list.at(0).demand > 0)
    {
        assert(!_list.at(0).adjacent.empty());
        path p = compute_shortest_path();
        augpath(p);
    }

    std::cout << "Min flow computed" << std::endl;
    //std::cout << (*this);

    std::vector<weight> potential(_list.size(), _invalid_cost);
    potential.at(1) = 0;

    for (size_t i = 0; i < _list.size(); ++i)
    {
        for (auto &l: _list)
        {
            for (auto &e: l.adjacent)
            {
                edge cur_edge = _edges[e];
                if (is_allowed(cur_edge, l.index) && potential.at(cur_edge.from) < _invalid_cost &&
                    potential.at(cur_edge.from) + cur_edge.cost < potential.at(cur_edge.to))
                {
                    potential.at(cur_edge.to) = potential.at(cur_edge.from) + cur_edge.cost;
                }
            }
        }
    }

    /*
    std::cout << "Potential:" << std::endl;
    for (size_t i = 0; i < _potential.size(); ++i)
    {
        std::cout << i << ": " << _potential.at(i) << std::endl;
    }
    */
    for(auto &edge : _edges)
    {
        if(edge.flow < edge.cap && potential_cost(edge.id, false) < 0)
        {
            std::cout << edge.from << " -> " << edge.to << std::endl;
            throw std::runtime_error("Invalid potential.");
        }
    }

    return true;
}


// This is O(n^2)-dijkstra. It suffices for the requested runtime and right now I am to lazy for something better.
path graph::compute_shortest_path()
{
    std::vector<weight> distances(_potential.size(), _invalid_cost);
    distances.at(0) = 0;
    path ret;
    std::vector<size_t> prev_edges(_list.size());
    std::vector<bool> fix(_list.size(), false);

    size_t min_index = 0;
    do
    {
        fix.at(min_index) = true;

        for (const auto &e: _list.at(min_index).adjacent)
        {
            const edge &cur_edge = _edges.at(e);
            if (cur_edge.from != min_index && cur_edge.flow == 0)
            {
                continue;
            }
            if (cur_edge.from == min_index && cur_edge.flow == cur_edge.cap)
            {
                continue;
            }
            weight new_dist = distances.at(min_index) + potential_cost(e, cur_edge.from != min_index);
            size_t neighbour = other_endpoint(e, min_index);

            if (!fix.at(neighbour) && new_dist < distances.at(neighbour))
            {
                distances.at(neighbour) = new_dist;
                prev_edges.at(neighbour) = e;
            }
        }

        min_index = _invalid_index;

        for (size_t i = 0; i < distances.size(); ++i)
        {
            if (!fix.at(i) && (min_index == _invalid_index || distances.at(i) < distances.at(min_index)))
            {
                min_index = i;
            }
        }
    }
    while (min_index != _invalid_index);

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
    while (cur != 0)
    {
        ret.push_front(prev_edges.at(cur));
        cur = other_endpoint(prev_edges.at(cur), cur);
    }

    return ret;
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

    return changed;
}


//TODO: This is bullshit; rewrite
weight graph::place(packing &pack)
{
    weight ret = 0;
    pos base = pack.get_rect(-1).get_pos(_dim);
    for(auto &n: _list)
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
                rectangle &rect = pack.get_rect(n.object_index);
                rect.base.coord(_dim) = base - _potential.at(n.index);
                rect.base.set = true;
                //std::cout << base << ", " << rect.base.coord(_dim) << " " << rect.get_max(_dim) << std::endl;
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

    ret.add_node(node(0, 0, node_type::source));
    ret.add_node(node(1, 0, node_type::chip_base));

    for (size_t i = 0; i < pack.get_num_rects(); ++i)
    {
        ret.add_node(node(ret.get_node_index(node_type::rect_node, i), i, node_type::rect_node));
        ret.add_bound_arcs(pack.get_rect(i));
    }

    for (size_t i = 0; i < pack.get_num_nets(); ++i)
    {
        ret.add_node(node(pack.get_net(i), ret.get_node_index(node_type::net_lower_node, i), true));
        ret.add_node(node(pack.get_net(i), ret.get_node_index(node_type::net_upper_node, i), false));
    }

    for (size_t i = 0; i < pack.get_num_nets(); ++i)
    {
        for (auto p: pack.get_net(i).pin_list)
        {
            ret.add_pin_arcs(p, i);
        }
    }

    std::vector<bool> smaller_neg_locus(pack.get_num_rects());

    for (size_t i = 0; i < pack.get_num_rects(); ++i)
    {
        std::fill(smaller_neg_locus.begin(), smaller_neg_locus.end(), false);

        auto first_neg = std::find(sp.negative_locus.begin(), sp.negative_locus.end(), i);
        ++first_neg;
        for (auto it = first_neg; it != sp.negative_locus.end(); it++)
        {
            smaller_neg_locus.at(*it) = true;
        }

        // TODO: This is shit
        switch (dim)
        {
            case dimension::x:
            {
                auto first_pos = std::find(sp.positive_locus.begin(), sp.positive_locus.end(), i);
                ++first_pos;
                for (auto it = first_pos; it != sp.positive_locus.end(); it++)
                {
                    if (smaller_neg_locus.at(*it))
                    {
                        ret.add_orientation_arcs(i, *it);
                    }
                }
                break;
            }
            case dimension::y:
            {
                auto first_pos = std::find(sp.positive_locus.rbegin(), sp.positive_locus.rend(), i);
                ++first_pos;
                for (auto it = first_pos; it != sp.positive_locus.rend(); it++)
                {
                    if (smaller_neg_locus.at(*it))
                    {
                        ret.add_orientation_arcs(i, *it);
                    }
                }
                break;
            }
        }
    }

    return ret;
}

void graph::add_arc(size_t from, size_t to, weight cost, weight flow, weight cap)
{
    node &from_node = _list.at(from);
    node &to_node = _list.at(to);

    from_node.adjacent.push_back(_edges.size());
    to_node.adjacent.push_back(_edges.size());
    from_node.demand -= flow;
    to_node.demand += flow;
    _edges.emplace_back(from, to, _edges.size(), cost, flow, cap);
}

size_t graph::get_node_index(node_type type, size_t index)
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
    }
}

void graph::add_bound_arcs(const rectangle &rect)
{
    size_t index = get_node_index(node_type::rect_node, (size_t) rect.id);
    size_t chip_base = get_node_index(node_type::chip_base);
    add_arc(chip_base, index, _pack.get_chip_base().get_pos(_dim));
    add_arc(index, chip_base, rect.get_dimension(_dim) - _pack.get_chip_base().get_max(_dim));
}

void graph::add_pin_arcs(const pin &p, size_t net_id, weight flow)
{
    pos rel_pin_pos = _pack.get_rect(p.index).get_relative_pin_position(p, _dim);
    size_t pin_index = get_node_index(node_type::rect_node, (size_t) p.index);
    add_arc(get_node_index(node_type::net_lower_node, net_id), pin_index, -rel_pin_pos, flow);
    add_arc(pin_index, get_node_index(node_type::net_upper_node, net_id), rel_pin_pos, flow);
}

void graph::add_orientation_arcs(size_t smaller, size_t bigger)
{
    add_arc(get_node_index(node_type::rect_node, smaller), get_node_index(node_type::rect_node, bigger),
            _pack.get_rect(smaller).get_dimension(_dim));
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
        if (_list.back().demand > 0)
        {
            add_arc(0, _list.size() - 1, 0, 0, _list.back().demand);
            _list.at(0).demand += _list.back().demand;
            _list.back().demand = 0;
        }
    }
}

size_t graph::other_endpoint(size_t edge_id, size_t first_node) const
{
    return _edges.at(edge_id).other_endpoint(first_node);
}

bool graph::is_allowed(const edge &e, size_t from) const
{
    if (e.from != from)
    {
        return e.flow < e.cap;
    }
    else
    {
        return e.flow > 0;
    }
}

size_t edge::other_endpoint(size_t first) const
{
    assert(first == from || first == to);
    return first != from ? from : to;
}
