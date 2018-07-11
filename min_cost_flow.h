#ifndef RECHTECKSPACKUNG_MIN_COST_FLOW_H
#define RECHTECKSPACKUNG_MIN_COST_FLOW_H

#include <vector>
#include <list>
#include <cassert>
#include <limits>
#include <algorithm> //min_element
#include "list_wrapper.h"
#include "packing.h"
#include "common.h"

static constexpr size_t _invalid_index = std::numeric_limits<size_t>::max();
static constexpr weight _invalid_cost = std::numeric_limits<weight>::max();

using node_id = size_t;
using edge_id = size_t;


/**
 * A structure which represents an edge of the graph. It represents the edge an its reversal and which one is meant can
 * only be inferred by the endpoint which is the source.
 */
struct edge
{
    const node_id from;
    const node_id to;
    const edge_id id;
    const weight cost;
    weight flow;
    const weight cap;

    edge(node_id from_, node_id to_, edge_id id_, weight cost_, weight cap_) :
            from(from_),
            to(to_),
            id(id_),
            cost(-cost_),
            flow(0),
            cap(cap_)
    {
        assert(from != to);
    }

    /**
     * Returns the other endpoint of this edge.
     * @param first The first endpoint.
     * @return The index of the endpoint which is not first.
     */
    node_id other_endpoint(node_id first) const;

    /**
     * The residual capacity of this edge. So the current flow value for the reverse variant and the capacity minus
     * the current flow for the forward variant.
     * @param from The node from which the edge is viewed. By this it can be inferred if the forward or reverse edge is
     * meant.
     * @return The residual capacity.
     */
    weight residual_cap(node_id from) const;
};

/**
 * The different types of nodes.
 */
enum class node_type
{
    source = 0,
    chip_base = 1,
    rect_node = 2,
    net_lower_node = 3,
    net_upper_node = 4
};

/**
 * Colours for the dot file.
 */
static std::string type_to_color[5] = {
        "black",
        "gold",
        "chartreuse",
        "crimson",
        "navy"
};

/**
 * This struct represents a node of our graph. It has a list with all adjacent edges (forward and reverse!).
 */
struct node
{
    const node_id index;

    // The index of the represented index. Not needed for the source and the chip base.
    const int object_index;
    weight demand;
    std::list<edge_id> adjacent;
    const node_type type;

    node(node_id index_, int object_index_, node_type type_) :
            index(index_),
            object_index(object_index_),
            demand(0),
            type(type_)
    {}

    node() :
            index(0),
            object_index(0),
            demand(0),
            type(node_type::source)
    {}

    node(const net &n, node_id index_, bool lower) :
            index(index_),
            object_index((int) n.index),
            demand(lower ? n.net_weight : -n.net_weight),
            type(lower ? node_type::net_lower_node : node_type::net_upper_node)
    {}
};

using adjlist = std::vector<node>;
using path = std::list<edge_id>;

class packing;
class sequence_pair;

class graph
{
    friend std::ostream &operator<<(std::ostream &out, const graph &g);

public:
    graph(packing &pack_, const dimension dim_) :
            _pack(pack_),
            _dim(dim_)
    {}

    /**
     * Computes the graph corresponding to the given pack.
     * @param pack The pack from which to obtain the rectangle and nets
     * @param dim The dimension which should be used
     * @param sp The sequence pair from which to obtain the orientation information.
     * @return The corresponding graph
     */
    static graph make_graph(packing &pack, dimension dim, sequence_pair sp);

    /**
     * Tries to compute a minimum flow on the graph. If there is circle of negative weight, the flow problem would be
     * unbounded, so we give up. This means that there is no valid packing with this sequence pair.
     * @return True if a minimum flow was computed, false if the instance contains a negative cycle.
     */
    bool compute_min_flow();

    /**
     * Places the rectangle of _pack according to the current _potential.
     */
    weight place();

    /**
     * Writes the current graph to a file which from which one can obtain a visualization by graphviz's dot program.
     * I wrote this function for debugging purposes and it is currently not used, but I saw no reason to remove it.
     * @param outfile The name of the file to which the graph is written
     * @param flow If false the whole graph is written, if false only the edges with flow are written.
     */
    void to_dot(std::string outfile, bool flow = false);

private:
    /**
     * Returns the potential edge cost with respect to the current _potential.
     * @param edge_index The edge of which the cost is computed.
     * @param reverse If true, the cost of the reverse edge is computed.
     * @return The potential edge cost.
     */
    weight potential_cost(edge_id edge_index, bool reverse) const;

    /**
     * Augments the flow on the given path by the given value. Paths always start at the source (node_id = 0). This
     * specification is necessary since we only pass the ids of the edges and not if they are to be traversed in the
     * reverse direction. If the start node is fixed this can be inferred. This invalidate the potential.
     * @param p The path to augment.
     * @param w The flow value by which we augment.
     */
    void augment_path(path &p, weight w);

    /**
     * We add the given node to the graph. If it has a positive demand, we also add edges to the source node, so we only
     * have one node with positive demand.
     * @param n The node to add.
     */
    void add_node(node &&n);

    /**
     * Adds an arc with the given parameters to the graph. The reverse edge is also directly added.
     * @param from
     * @param to
     * @param cost
     * @param cap
     */
    void add_arc(node_id from, node_id to, weight cost, weight cap = _invalid_cost);

    /**
     * Adds the edges which represent the constraint that the rectangle has to lay in the chip base.
     * @param rect The rectangle for which to add the edges.
     */
    void add_bound_edges(const rectangle &rect);

    /**
     * Adds the edges which represent the constraint that the pin has to lay between the lower and upper bounds of a
     * net to which it belongs. This has to be called for all nets to which the pin belongs.
     * @param p The pin for which to add the edges
     * @param net_id The id of one net to which the pin belongs.
     */
    void add_pin_edges(const pin &p, size_t net_id);

    /**
     * Adds the edges which represent the constraint that one rectangle lies to a side of another.
     * @param smaller The id of the rectangle on the left/below
     * @param bigger The id of the rectangle on the right/above
     */
    void add_orientation_edges(size_t smaller, size_t bigger);

    /**
     * Returns whether the flow on the edge can still be augmented.
     * @param e The edge for which to check.
     * @param from The id of the node which is the source of the edge. This is needed to infer if we have a forward or
     *  a reverse edge.
     * @return True if residual capacity of the edge is positive.
     */
    bool is_allowed(const edge &e, node_id from) const;

    /**
     * Returns the index of the node which corresponds to the described object.
     * @param type The type of the node to return.
     * @param index The index of the object (so rectangle id or net id) which is meant. Not really needed for source and
     * chip base.
     * @return The index of the corresponding node.
     */
    node_id get_node_index(node_type type, size_t index = 0) const;

    /**
     * Returns the index of the other endpoint of the edge.
     * @param edge_index The index of the edge.
     * @param first_node The index of the first node.
     * @return The index of the endpoint of the edge with index edge_id which is not not first_node.
     */
    node_id other_endpoint(edge_id edge_index, node_id first_node) const;

    /**
     * Computes a shortest path from the source node to an arbitrary node with negative demand. This also updates
     * _potential which maybe invalid since the last flow augmentation.
     * @param ret The path in which we save the path.
     * @return The maximal capacity on this path.
     */
    weight compute_shortest_path(path &ret);

    /**
     * Computes a starting potential with the Moore-Bellmann-Ford-Algorithm for graphs with conservative edge weights.
     * @return False if there is a negative cycle, i.e. if the edge weights are not conservative.
     */
    bool compute_starting_potential();

    /**
     * Adds all nodes which belong to _pack;
     */
    void add_all_nodes();

    /**
     * Adds all orientation edges which contain rect_index as smaller rectangle.
     * @tparam Iterator Depending on the dimension we need to call this on the postivie locus of our sequence pair
     * in different directions. This seemed like a not too terribly hacky way to make this work with forward and reverse
     * iterators.
     * @param rect_index The rectangle which is the smaller for all orientation edges.
     * @param begin The beginning of the positive locus.
     * @param end The end of the positive. We should not reach it but it seems better to check this.
     * @param smaller_negative_locus Is true at index i iff the rectangle with index i is left of rect_index in the
     * negative locus.
     */
    template<class Iterator>
    void add_all_orientations(size_t rect_index, const Iterator &begin, const Iterator &end,
                              const std::vector<bool> &smaller_negative_locus);

    adjlist _list;
    std::vector<edge> _edges;
    std::vector<weight> _potential;
    packing &_pack;
    const dimension _dim;
};


#endif //RECHTECKSPACKUNG_MIN_COST_FLOW_H
