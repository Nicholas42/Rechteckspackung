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
};

struct node
{
    weight demand;
    std::list<edge> adjacent;
};

using adjlist = std::vector<node>;
using path = std::list<edge>;

class graph
{
public:
    static graph make_graph();

    void augpath(const path &p, weight w = 0);
    weight potential_cost(const edge &e) const;
    void compute_min_flow();

private:
    adjlist _list;
    std::vector<weight> _potential;
    std::vector<weight > _flow;
    path compute_shortest_path(size_t from);
    void compute_starting_potential();
};

#endif //RECHTECKSPACKUNG_MIN_COST_FLOW_H
