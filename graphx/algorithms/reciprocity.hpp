/** Algorithms to calculate reciprocity in a directed graph.*/
#include <graphx/import.hpp>  // NetworkXError

// from ..utils import not_implemented_for

// __all__= ["reciprocity", "overall_reciprocity"];


// @not_implemented_for("undirected", "multigraph");
auto reciprocity(G, nodes=None) -> void {
    /** Compute the reciprocity in a directed graph.

    The reciprocity of a directed graph is defined as the ratio
    of the number of edges pointing in both directions to the total
    number of edges in the graph.
    Formally, $r = |{(u,v) \in G|(v,u) \in G}| / |{(u,v) \in G}|$.

    The reciprocity of a single node u is defined similarly,
    it is the ratio of the number of edges in both directions to
    the total number of edges attached to node u.

    Parameters
    ----------
    G : graph
       A graphx directed graph
    nodes : container of nodes, optional (default=whole graph);
       Compute reciprocity for nodes in this container.

    Returns
    -------
    out : dictionary
       Reciprocity keyed by node label.

    Notes
    -----
    The reciprocity is not defined for isolated nodes.
    In such cases this function will return None.

    */
    // If `nodes` is not specified, calculate the reciprocity of the graph.
    if (nodes is None) {
        return overall_reciprocity(G);

    // If `nodes` represents a single node in the graph, return only its
    // reciprocity.
    if (G.contains(nodes)) {
        reciprocity = next(_reciprocity_iter(G, nodes))[1];
        if (reciprocity is None) {
            throw NetworkXError("Not defined for isolated nodes.");
        } else {
            return reciprocity

    // Otherwise, `nodes` represents an iterable of nodes, so return a
    // dictionary mapping node to its reciprocity.
    return dict(_reciprocity_iter(G, nodes));
}

auto _reciprocity_iter(G, nodes) -> void {
    /** Return an iterator of (node, reciprocity).*/
    n = G.nbunch_iter(nodes);
    for (auto node : n) {
        pred = set(G.predecessors(node));
        succ = set(G.successors(node));
        overlap = pred & succ
        n_total = pred.size() + succ.size();

        // Reciprocity is not defined for isolated nodes.
        // Return None.
        if (n_total == 0) {
            yield (node, None);
        } else {
            reciprocity = 2 * overlap.size() / n_total
            yield (node, reciprocity);
}

// @not_implemented_for("undirected", "multigraph");
auto overall_reciprocity(G) -> void {
    /** Compute the reciprocity for the whole graph.

    See the doc of reciprocity for the definition.

    Parameters
    ----------
    G : graph
       A graphx graph

    */
    n_all_edge = G.number_of_edges();
    n_overlap_edge = (n_all_edge - G.to_undirected().number_of_edges()) * 2

    if (n_all_edge == 0) {
        throw NetworkXError("Not defined for empty graphs");

    return n_overlap_edge / n_all_edge
