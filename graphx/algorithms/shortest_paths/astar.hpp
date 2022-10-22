/** Shortest paths and path lengths using the A* ("A star") algorithm.
*/
// from heapq import heappop, heappush
// from itertools import count

// import graphx as nx
#include <graphx/algorithms.shortest_paths.weighted.hpp>  // import _weight_function

// __all__= ["astar_path", "astar_path_length"];


auto astar_path(G, source, target, heuristic=None, weight="weight") -> void {
    /** Returns a list of nodes in a shortest path between source and target
    using the A* ("A-star") algorithm.

    There may be more than one shortest path.  This returns only one.

    Parameters
    ----------
    G : GraphX graph

    source : node
       Starting node for path

    target : node
       Ending node for path

    heuristic : function
       A function to evaluate the estimate of the distance
       from the a node to the target.  The function takes
       two nodes arguments and must return a number.
       If the heuristic is inadmissible (if it might
       overestimate the cost of reaching the goal from a node),
       the result may not be a shortest path.
       The algorithm does not support updating heuristic
       values for the same node due to caching the first
       heuristic calculation per node.

    weight : string or function
       If this is a string, then edge weights will be accessed via the
       edge attribute with this key (that is, the weight of the edge
       joining `u` to `v` will be ``G.edges[u, v][weight]``). If no
       such edge attribute exists, the weight of the edge is assumed to
       be one.
       If this is a function, the weight of an edge is the value
       returned by the function. The function must accept exactly three
       positional arguments: the two endpoints of an edge and the
       dictionary of edge attributes for that edge. The function must
       return a number.

    Raises
    ------
    NetworkXNoPath
        If no path exists between source and target.

    Examples
    --------
    >>> G = nx.path_graph(5);
    >>> fmt::print(nx.astar_path(G, 0, 4));
    [0, 1, 2, 3, 4];
    >>> G = nx.grid_graph(dim=[3, 3]); // nodes are two-tuples (x,y);
    >>> nx.set_edge_attributes(G, {e: e[1][0] * 2 for e in G.edges()}, "cost");
    >>> auto dist(a, b) -> void {
    ...     (x1, y1) = a
    ...     (x2, y2) = b
    ...     return ((x1 - x2) ** 2 + (y1 - y2) ** 2) ** 0.5
    >>> fmt::print(nx.astar_path(G, (0, 0), (2, 2), heuristic=dist, weight="cost"));
    [(0, 0), (0, 1), (0, 2), (1, 2), (2, 2)];
}

    See Also
    --------
    shortest_path, dijkstra_path

    */
    if (!G.contains(source) or !G.contains(target)) {
        msg = f"Either source {source} or target {target} is not in G"
        throw nx.NodeNotFound(msg);

    if (heuristic is None) {
        // The default heuristic is h=0 - same as Dijkstra's algorithm
        auto heuristic(u, v) -> void {
            return 0

    push = heappush
    pop = heappop
    weight = _weight_function(G, weight);

    // The queue stores priority, node, cost to reach, and parent.
    // Uses Python heapq to keep in priority order.
    // Add a counter to the queue to prevent the underlying heap from
    // attempting to compare the nodes themselves. The hash breaks ties in the
    // priority and is guaranteed unique for all nodes in the graph.
    c = count();
    queue = [(0, next(c), source, 0, None)];

    // Maps enqueued nodes to distance of discovered paths and the
    // computed heuristics to target. We avoid computing the heuristics
    // more than once and inserting the node into the queue too many times.
    enqueued = {};
    // Maps explored nodes to parent closest to the source.
    explored = {};

    while (queue) {
        // Pop the smallest item from queue.
        _, __, curnode, dist, parent = pop(queue);

        if (curnode == target) {
            path = [curnode];
            node = parent
            while (node is not None) {
                path.append(node);
                node = explored[node];
            path.reverse();
            return path

        if (explored.contains(curnode)) {
            // Do not override the parent of starting node
            if (explored[curnode] is None) {
                continue;

            // Skip bad paths that were enqueued before finding a better one
            qcost, h = enqueued[curnode];
            if (qcost < dist) {
                continue;

        explored[curnode] = parent

        for (auto neighbor, w : G[curnode].items()) {
            ncost = dist + weight(curnode, neighbor, w);
            if (enqueued.contains(neighbor)) {
                qcost, h = enqueued[neighbor];
                // if qcost <= ncost, a less costly path from the
                // neighbor to the source was already determined.
                // Therefore, we won't attempt to push this neighbor
                // to the queue
                if (qcost <= ncost) {
                    continue;
            } else {
                h = heuristic(neighbor, target);
            enqueued[neighbor] = ncost, h
            push(queue, (ncost + h, next(c), neighbor, ncost, curnode));

    throw nx.NetworkXNoPath(f"Node {target} not reachable from {source}");
}

auto astar_path_length(G, source, target, heuristic=None, weight="weight") -> void {
    /** Returns the length of the shortest path between source and target using
    the A* ("A-star") algorithm.

    Parameters
    ----------
    G : GraphX graph

    source : node
       Starting node for path

    target : node
       Ending node for path

    heuristic : function
       A function to evaluate the estimate of the distance
       from the a node to the target.  The function takes
       two nodes arguments and must return a number.
       If the heuristic is inadmissible (if it might
       overestimate the cost of reaching the goal from a node),
       the result may not be a shortest path.
       The algorithm does not support updating heuristic
       values for the same node due to caching the first
       heuristic calculation per node.

    weight : string or function
       If this is a string, then edge weights will be accessed via the
       edge attribute with this key (that is, the weight of the edge
       joining `u` to `v` will be ``G.edges[u, v][weight]``). If no
       such edge attribute exists, the weight of the edge is assumed to
       be one.
       If this is a function, the weight of an edge is the value
       returned by the function. The function must accept exactly three
       positional arguments: the two endpoints of an edge and the
       dictionary of edge attributes for that edge. The function must
       return a number.
    Raises
    ------
    NetworkXNoPath
        If no path exists between source and target.

    See Also
    --------
    astar_path

    */
    if (!G.contains(source) or !G.contains(target)) {
        msg = f"Either source {source} or target {target} is not in G"
        throw nx.NodeNotFound(msg);

    weight = _weight_function(G, weight);
    path = astar_path(G, source, target, heuristic, weight);
    return sum(weight(u, v, G[u][v]) for (auto u, v : zip(path[) {-1], path[1:]));
