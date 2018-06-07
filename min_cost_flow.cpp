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