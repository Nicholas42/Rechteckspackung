#ifndef RECHTECKSPACKUNG_MIN_COST_FLOW_H
#define RECHTECKSPACKUNG_MIN_COST_FLOW_H

#include <vector>
#include <list>
#include <cassert>
#include <limits>
#include <algorithm> //min_element
#include "packing.h"
#include "common.h"

static constexpr size_t _invalid_index = std::numeric_limits<size_t>::max();
static constexpr weight _invalid_cost = std::numeric_limits<weight>::max();


struct edge
{
    const size_t from;
    const size_t to;
    const size_t id;
    const weight cost;
    weight flow;
    const weight cap;

    edge(size_t from_, size_t to_, size_t id_, weight cost_, weight flow_ = 0, weight cap_ = _invalid_cost) :
            from(from_),
            to(to_),
            id(id_),
            cost(-cost_),
            flow(flow_),
            cap(cap_)
    {
        assert(from != to);
    }

    size_t other_endpoint(size_t first) const;
};

enum class node_type
{
    source = 0,
    chip_base = 1,
    rect_node = 2,
    net_lower_node = 3,
    net_upper_node = 4
};

static std::string type_to_color[5] = {
        "black",
        "gold",
        "chartreuse",
        "crimson",
        "navy"
};

struct node
{
    const size_t index;
    const int object_index;
    weight demand;
    std::list<size_t> adjacent;
    const node_type type;

    node(size_t index_, int object_index_, node_type type_) :
            index(index_),
            object_index(object_index_),
            demand(0),
            type(type_)
    {}

    node(size_t index_, weight demand_) :
            index(index_),
            object_index(0),
            demand(demand_),
            type(node_type::source)
    {}

    node(const net &n, size_t index_, bool lower) :
            index(index_),
            object_index(n.index),
            demand(lower ? n.net_weight : -n.net_weight),
            type(lower ? node_type::net_lower_node : node_type::net_upper_node)
    {}
};

using adjlist = std::vector<node>;
using path = std::list<size_t>;

class packing;

class graph
{
    friend std::ostream &operator<<(std::ostream &out, const graph &g);

public:
    graph(packing &pack_, const dimension dim_) :
            _pack(pack_),
            _dim(dim_)
    {}

    static graph make_graph(packing &pack, dimension dim, sequence_pair sp);

    weight potential_cost(size_t edge_id, bool reverse) const;
    bool compute_min_flow();
    weight place(packing &pack);
    void to_dot(std::string outfile, bool flow = false);

private:
    void augpath(path &p, weight w = 0);
    void add_node(node &&n);
    void add_arc(size_t from, size_t to, weight cost, weight flow = 0, weight cap = _invalid_cost);
    void add_bound_arcs(const rectangle &rect);
    void add_pin_arcs(const pin &p, size_t net_id, weight flow = 0);
    void add_orientation_arcs(size_t smaller, size_t bigger);
    bool is_allowed(const edge &e, size_t from) const;
    size_t get_node_index(node_type type, size_t index = 0);
    size_t other_endpoint(size_t edge_id, size_t first_node) const;
    adjlist _list;
    std::vector<edge> _edges;
    std::vector<weight> _potential;
    const packing &_pack;
    const dimension _dim;

    path compute_shortest_path();
    bool compute_starting_potential();
};


#endif //RECHTECKSPACKUNG_MIN_COST_FLOW_H
