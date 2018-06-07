#include "min_cost_flow.h"


void graph::augpath(const path &p, weight w)
{
    assert(!p.empty());

    size_t start_node = p.front().reverse ? p.front().to : p.front().from;
    size_t end_node = p.back().reverse ? p.back().from : p.back().to;
    if (w == 0)
    {
        w = std::min(_list[start_node].demand, -_list[end_node].demand);
        for (auto const &e: p)
        {
            if (e.reverse)
            {
                w = std::min(w, _flow[e.id]);
            }
        }
    }

    for (auto const &e: p)
    {
        if (e.reverse)
        {
            _flow[e.id] -= w;
        } else
        {
            _flow[e.id] += w;
        }

        assert(_flow[e.id] >= 0);
    }

    assert(w <= _list[start_node].demand);
    assert(w <= -_list[end_node].demand);
    _list[start_node].demand -= w;
    _list[end_node].demand += w;
}

weight graph::potential_cost(const edge &e) const
{
    return e.cost + _potential[e.from] - _potential[e.to];
}

void graph::compute_min_flow()
{
    for (auto &node : _list)
    {
        while (node.demand > 0)
        {
            assert(!node.adjacent.empty());
            path p = compute_shortest_path(node.adjacent.front().from);
            augpath(p);
        }
    }
}


// This is O(n^2)-dijkstra. It suffices for the requested runtime and right now I am to lazy for something better.
path graph::compute_shortest_path(size_t from)
{
    std::vector<weight> distances(_potential.size(), _invalid_cost);
    distances[from] = 0;
    path ret;
    std::vector<size_t> prev_nodes(_list.size(), _invalid_index);
    std::vector<bool> fix(_list.size(), false);

    std::vector<weight>::iterator min_it;
    while (_invalid_cost != *(min_it = std::min_element(distances.begin(), distances.end())))
    {
        size_t min_index = std::distance(distances.begin(), min_it);

        fix[min_index] = true;

        for (auto &e: _list[min_index].adjacent)
        {
            if (e.reverse && _flow[e.id] == 0)
            {
                continue;
            }
            weight new_dist = distances[min_index] + potential_cost(e);
            if (!fix[e.to] && new_dist < distances[e.to])
            {
                distances[e.to] = new_dist;
                prev_nodes[e.to] = min_index;
            }
        }
    }

    size_t t = _invalid_index;

    for (size_t i = 0; i < _list.size(); ++i)
    {
        assert(distances[i] < _invalid_cost);
        _potential[i] += distances[i];
        if (_list[i].demand < 0)
        {
            t = i;
        }
    }

    assert(t != _invalid_index);

    weight max_flow = std::min(_list[from].demand, -_list[t].demand);
    size_t cur = t;
    while (prev_nodes[cur] != _invalid_index)
    {
        // This looks stupid, but I haven't got a better idea.
        cur = prev_nodes[cur];
        ret.push_front(*std::find_if(_list[cur].adjacent.begin(), _list[cur].adjacent.end(), [t](const edge &e)
        {
            return e.to == t;
        }));

        t = cur;
    }

    return ret;
}

void graph::compute_starting_potential()
{
    _potential.resize(_list.size(), _invalid_cost);
    _potential[0] = 0;

    for (size_t i = 0; i < _list.size(); ++i)
    {
        for (auto &l: _list)
        {
            for (auto &e: l.adjacent)
            {
                if (!e.reverse && _potential[e.from] < _invalid_cost && _potential[e.from] + e.cost < _potential[e.to])
                {
                    _potential[e.to] = _potential[e.from] + e.cost;
                }
            }
        }
    }
}


//TODO: do it for x and y
void graph::place(packing &pack)
{
    std::vector<weight> distance(_list.size(), _invalid_cost);
    std::list<size_t> queue;
    size_t cur_node = 0;
    distance[cur_node] = pack.get_x_min();
    queue.push_front(cur_node);

    while (!queue.empty())
    {
        cur_node = queue.front();
        queue.pop_front();

        if (0 < cur_node && cur_node <= pack.get_num_rects())
        {
            pack.get_rect(cur_node - 1).x = distance[cur_node];
        }

        for (edge &cur_edge: _list[cur_node].adjacent)
        {
            size_t neighbor = cur_edge.reverse ? cur_edge.from : cur_edge.to;
            if ((cur_edge.reverse || _flow[cur_edge.id] > 0) && distance[neighbor] == _invalid_cost)
            {
                distance[neighbor] = distance[cur_node] + cur_edge.cost;
                queue.push_back(neighbor);
            }
        }
    }
}

graph graph::make_graph(packing &pack, dimension dim, sequence_pair sp)
{
    graph ret;

    ret._list.reserve(1 + pack.get_num_rects() + 2 * pack.get_num_nets());
    ret._list.emplace_back(0);

    for (size_t i = 1; i <= pack.get_num_rects(); ++i)
    {
        ret._list.emplace_back(i);
        ret.add_arc(0, i, pack.get_min_pos(dim));
        ret.add_arc(i, 0, pack.get_rect(i - 1).get_dimension(dim) - pack.get_max_pos(dim));
    }

    for (size_t i = 0; i < pack.get_num_nets(); ++i)
    {
        ret._list.emplace_back(ret._list.size(), pack.get_net(i).net_weight);
        ret._list.emplace_back(ret._list.size(), -pack.get_net(i).net_weight);
    }

    ret._potential.resize(ret._list.size(), 0);

    for (size_t i = 0; i < pack.get_num_nets(); ++i)
    {
        size_t net_id = 1 + pack.get_num_rects() + 2 * i;
        for(auto p: pack.get_net(i).pin_list)
        {
            if(p.index == -1)
            {
                // The other arc should be exactly the reverse arc.
                // TODO: check this.
                ret.add_arc(net_id, 0, -p.get_pos(dim));
                ret.add_arc(0, net_id, -p.get_pos(dim));
            }
            else
            {
                pos rel_pin_pos = pack.get_rect(p.index).get_relative_pin_position(p, dim);
                ret.add_arc(net_id, p.index + 1, -rel_pin_pos);
                ret.add_arc(p.index + 1, net_id + 1, rel_pin_pos);
            }
        }
    }

    // TODO: at stuff from sequence pair...
}

void graph::add_arc(size_t from, size_t to, weight cost)
{
    _list[from].adjacent.emplace_back(from, to, _num_edges++, cost, false);
    _list[to].adjacent.emplace_back(to, from, _num_edges++, -cost, true);
}
