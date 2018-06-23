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
static constexpr  weight _invalid_cost = std::numeric_limits<weight>::max();


// TODO: one could save memory by putting everything short of id and reverse in vector
struct edge
{
    size_t from;
    size_t to;
    size_t id;
    weight cost;
    bool reverse;

    edge(size_t from_, size_t to_, size_t id_, weight cost_, bool reverse_) :
            from(from_), to(to_), id(id_), cost(cost_), reverse(reverse_)
    {}
};

enum class node_type
{
    chip_base = 0,
    rect_node = 1,
    net_lower_node = 2,
    net_upper_node = 3
};

struct node
{
    size_t index;
    weight demand;
    std::list<edge> adjacent;
    node_type type;


    explicit node(size_t index_) :
            index(index_),
            demand(0),
            type(index == 0 ? node_type::chip_base : node_type::rect_node)
    {}

    node(size_t index_, weight demand_) :
            index(index_),
            demand(demand_)
    {}

    node(const net& n, size_t index_, bool lower) :
            index(index_),
            demand(lower ? n.net_weight : -n.net_weight),
            type(lower ? node_type::net_lower_node : node_type::net_upper_node)
    {}
};

using adjlist = std::vector<node>;
using path = std::list<edge>;

class graph
{
public:
    graph(const packing &pack_, const dimension dim_) :
            pack(pack_),
            dim(dim_)
    {}

    static graph make_graph(packing &pack, dimension dim, sequence_pair sp);

    void augpath(const path &p, weight w = 0);
    weight potential_cost(const edge &e) const;
    void compute_min_flow();
    void compute_final_potential();
    void place(packing &pack);
    void add_arc(size_t from, size_t to, weight cost);
    void add_bound_arcs(const rectangle &rect);
    void add_pin_arcs(const pin &p, size_t net_id);
    void add_orientation_arcs(size_t smaller, size_t bigger);
    size_t get_node_index(node_type type, size_t index);

private:
    adjlist _list;
    std::vector<weight> _potential;
    std::vector<weight > _flow;
    size_t _num_edges = 0;
    const packing &pack;
    const dimension dim;

    path compute_shortest_path(size_t from);
    void compute_starting_potential();
};

#endif //RECHTECKSPACKUNG_MIN_COST_FLOW_H
