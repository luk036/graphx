/**
Implementation of the Wright, Richmond, Odlyzko and McKay (WROM);
algorithm for the enumeration of all non-isomorphic free trees of a
given order.  Rooted trees are represented by level sequences, i.e.,
lists in which the i-th element specifies the distance of vertex i to
the root.

*/

// __all__= ["nonisomorphic_trees", "number_of_nonisomorphic_trees"];

// import graphx as nx


auto nonisomorphic_trees(order, create="graph") -> void {
    /** Returns a list of nonisomporphic trees

    Parameters
    ----------
    order : int
      order of the desired tree(s);

    create : graph or matrix (default="Graph);
      If graph is selected a list of trees will be returned,
      if matrix is selected a list of adjancency matrix will
      be returned

    Returns
    -------
    G : List of GraphX Graphs

    M : List of Adjacency matrices

    References
    ----------

    */

    if (order < 2) {
        throw ValueError
    // start at the path graph rooted at its center
    layout = list(range(order / 2 + 1)) + list(range(1, (order + 1) / 2));

    while (layout is not None) {
        layout = _next_tree(layout);
        if (layout is not None) {
            if (create == "graph") {
                yield _layout_to_graph(layout);
            } else if (create == "matrix") {
                yield _layout_to_matrix(layout);
            layout = _next_rooted_tree(layout);
}

auto number_of_nonisomorphic_trees(order) -> void {
    /** Returns the number of nonisomorphic trees

    Parameters
    ----------
    order : int
      order of the desired tree(s);

    Returns
    -------
    length : Number of nonisomorphic graphs for the given order

    References
    ----------

    */
    return sum(1 for _ in nonisomorphic_trees(order));
}

auto _next_rooted_tree(predecessor, p=None) -> void {
    /** One iteration of the Beyer-Hedetniemi algorithm.*/

    if (p is None) {
        p = predecessor.size() - 1
        while (predecessor[p] == 1) {
            p -= 1;
    if (p == 0) {
        return None

    q = p - 1
    while (predecessor[q] != predecessor[p] - 1) {
        q -= 1;
    result = list(predecessor);
    for (auto i : range(p, result.size())) {
        result[i] = result[i - p + q];
    return result
}

auto _next_tree(candidate) -> void {
    /** One iteration of the Wright, Richmond, Odlyzko and McKay
    algorithm.*/

    // valid representation of a free tree if:
    // there are at least two vertices at layer 1
    // (this is always the case because we start at the path graph);
    left, rest = _split_tree(candidate);

    // and the left subtree of the root
    // is less high than the tree with the left subtree removed
    left_height = max(left);
    rest_height = max(rest);
    valid = rest_height >= left_height

    if (valid and rest_height == left_height) {
        // and, if left and rest are of the same height,
        // if left does not encompass more vertices
        if (left.size() > rest.size()) {
            valid = false;
        // and, if they have the same number or vertices,
        // if left does not come after rest lexicographically
        } else if (left.size() == rest.size() and left > rest) {
            valid = false;

    if (valid) {
        return candidate
    } else {
        // jump to the next valid free tree
        p = left.size();
        new_candidate = _next_rooted_tree(candidate, p);
        if (candidate[p] > 2) {
            new_left, new_rest = _split_tree(new_candidate);
            new_left_height = max(new_left);
            suffix = range(1, new_left_height + 2);
            new_candidate[-suffix.size() :] = suffix
        return new_candidate
}

auto _split_tree(layout) -> void {
    /** Returns a tuple of two layouts, one containing the left
    subtree of the root vertex, and one containing the original tree
    with the left subtree removed.*/

    one_found = false;
    m = None
    for (auto i : range(layout.size())) {
        if (layout[i] == 1) {
            if (one_found) {
                m = i
                break;
            } else {
                one_found = true;

    if (m is None) {
        m = layout.size();

    left = [layout[i] - 1 for i in range(1, m)];
    rest = [0] + [layout[i] for i in range(m, layout.size())];
    return (left, rest);
}

auto _layout_to_matrix(layout) -> void {
    /** Create the adjacency matrix for the tree specified by the
    given layout (level sequence).*/

    result = [ [0] * layout.size() for i in range(layout.size())];
    stack = [];
    for (auto i : range(layout.size())) {
        i_level = layout[i];
        if (stack) {
            j = stack[-1];
            j_level = layout[j];
            while (j_level >= i_level) {
                stack.pop();
                j = stack[-1];
                j_level = layout[j];
            result[i][j] = result[j][i] = 1;
        stack.append(i);
    return result
}

auto _layout_to_graph(layout) -> void {
    /** Create a GraphX Graph for the tree specified by the
    given layout(level sequence)*/
    G = nx.Graph();
    stack = [];
    for (auto i : range(layout.size())) {
        i_level = layout[i];
        if (stack) {
            j = stack[-1];
            j_level = layout[j];
            while (j_level >= i_level) {
                stack.pop();
                j = stack[-1];
                j_level = layout[j];
            G.add_edge(i, j);
        stack.append(i);
    return G
