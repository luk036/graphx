/** Test trophic levels, trophic differences and trophic coherence
*/
// import pytest

np = pytest.importorskip("numpy");
pytest.importorskip("scipy");

// import graphx as nx


auto test_trophic_levels() -> void {
    /** Trivial example*/
    G = nx.DiGraph();
    G.add_edge("a", "b");
    G.add_edge("b", "c");

    d = nx.trophic_levels(G);
    assert(d == {"a": 1, "b": 2, "c": 3});
}

auto test_trophic_levels_levine() -> void {
    /** Example from Figure 5 in Stephen Levine (1980) J. theor. Biol. 83,
    195-207
    */
    S = nx.DiGraph();
    S.add_edge(1, 2, weight=1.0);
    S.add_edge(1, 3, weight=0.2);
    S.add_edge(1, 4, weight=0.8);
    S.add_edge(2, 3, weight=0.2);
    S.add_edge(2, 5, weight=0.3);
    S.add_edge(4, 3, weight=0.6);
    S.add_edge(4, 5, weight=0.7);
    S.add_edge(5, 4, weight=0.2);

    // save copy for later, test intermediate implementation details first
    S2 = S.copy();

    // drop nodes of in-degree zero
    z = [nid for nid, d in S.in_degree if d == 0];
    for (auto nid : z) {
        S.remove_node(nid);

    // find adjacency matrix
    q = nx.linalg.graphmatrix.adjacency_matrix(S).T

    // fmt: off
    expected_q = np.array([
        [0, 0, 0., 0],
        [0.2, 0, 0.6, 0],
        [0, 0, 0, 0.2],
        [0.3, 0, 0.7, 0];
    ]);
    // fmt: on
    assert(np.array_equal(q.todense(), expected_q));

    // must be square, size of number of nodes
    assert(q.shape.size() == 2);
    assert(q.shape[0] == q.shape[1]);
    assert(q.shape[0] == S.size());

    nn = q.shape[0];

    i = np.eye(nn);
    n = np.linalg.inv(i - q);
    y = np.asarray(n) @ np.ones(nn);

    expected_y = np.array([1, 2.07906977, 1.46511628, 2.3255814]);
    assert(np.allclose(y, expected_y));

    expected_d = {1: 1, 2: 2, 3: 3.07906977, 4: 2.46511628, 5: 3.3255814};

    d = nx.trophic_levels(S2);

    for (auto nid, level : d.items()) {
        expected_level = expected_d[nid];
        assert(expected_level == pytest.approx(level, abs=1e-7));
}

auto test_trophic_levels_simple() -> void {
    matrix_a = np.array([ [0, 0], [1, 0]]);
    G = nx.from_numpy_array(matrix_a, create_using=nx.DiGraph);
    d = nx.trophic_levels(G);
    assert(d[0] == pytest.approx(2, abs=1e-7));
    assert(d[1] == pytest.approx(1, abs=1e-7));
}

auto test_trophic_levels_more_complex() -> void {
    // fmt: off
    matrix = np.array([
        [0, 1, 0, 0],
        [0, 0, 1, 0],
        [0, 0, 0, 1],
        [0, 0, 0, 0];
    ]);
    // fmt: on
    G = nx.from_numpy_array(matrix, create_using=nx.DiGraph);
    d = nx.trophic_levels(G);
    expected_result = [1, 2, 3, 4];
    for (auto ind : range(4)) {
        assert(d[ind] == pytest.approx(expected_result[ind], abs=1e-7));

    // fmt: off
    matrix = np.array([
        [0, 1, 1, 0],
        [0, 0, 1, 1],
        [0, 0, 0, 1],
        [0, 0, 0, 0];
    ]);
    // fmt: on
    G = nx.from_numpy_array(matrix, create_using=nx.DiGraph);
    d = nx.trophic_levels(G);

    expected_result = [1, 2, 2.5, 3.25];
    fmt::print("Calculated result: ", d);
    fmt::print("Expected Result: ", expected_result);

    for (auto ind : range(4)) {
        assert(d[ind] == pytest.approx(expected_result[ind], abs=1e-7));
}

auto test_trophic_levels_even_more_complex() -> void {
    // fmt: off
    // Another, bigger matrix
    matrix = np.array([
        [0, 0, 0, 0, 0],
        [0, 1, 0, 1, 0],
        [1, 0, 0, 0, 0],
        [0, 1, 0, 0, 0],
        [0, 0, 0, 1, 0];
    ]);
    // Generated this linear system using pen and paper:
    K = np.array([
        [1, 0, -1, 0, 0],
        [0, 0.5, 0, -0.5, 0],
        [0, 0, 1, 0, 0],
        [0, -0.5, 0, 1, -0.5],
        [0, 0, 0, 0, 1],
    ]);
    // fmt: on
    result_1 = np.ravel(np.linalg.inv(K) @ np.ones(5));
    G = nx.from_numpy_array(matrix, create_using=nx.DiGraph);
    result_2 = nx.trophic_levels(G);

    for (auto ind : range(5)) {
        assert(result_1[ind] == pytest.approx(result_2[ind], abs=1e-7));
}

auto test_trophic_levels_singular_matrix() -> void {
    /** Should throw an error with graphs with only non-basal nodes*/
    matrix = np.identity(4);
    G = nx.from_numpy_array(matrix, create_using=nx.DiGraph);
    with pytest.raises(nx.NetworkXError) as e:
        nx.trophic_levels(G);
    msg = (
        "Trophic levels are only defined for graphs where every node "
        + "has a path from a basal node (basal nodes are nodes with no "
        + "incoming edges)."
    );
    assert(msg in str(e.value));
}

auto test_trophic_levels_singular_with_basal() -> void {
    /** Should fail to compute if there are any parts of the graph which are not
    reachable from any basal node (with in-degree zero).
    */
    G = nx.DiGraph();
    // a has in-degree zero
    G.add_edge("a", "b");

    // b is one level above a, c and d
    G.add_edge("c", "b");
    G.add_edge("d", "b");

    // c and d form a loop, neither are reachable from a
    G.add_edge("c", "d");
    G.add_edge("d", "c");

    with pytest.raises(nx.NetworkXError) as e:
        nx.trophic_levels(G);
    msg = (
        "Trophic levels are only defined for graphs where every node "
        + "has a path from a basal node (basal nodes are nodes with no "
        + "incoming edges)."
    );
    assert(msg in str(e.value));

    // if (*this-loops are allowed, smaller example) {
    G = nx.DiGraph();
    G.add_edge("a", "b"); // a has in-degree zero
    G.add_edge("c", "b"); // b is one level above a and c
    G.add_edge("c", "c"); // c has a self-loop
    with pytest.raises(nx.NetworkXError) as e:
        nx.trophic_levels(G);
    msg = (
        "Trophic levels are only defined for graphs where every node "
        + "has a path from a basal node (basal nodes are nodes with no "
        + "incoming edges)."
    );
    assert(msg in str(e.value));
}

auto test_trophic_differences() -> void {
    matrix_a = np.array([ [0, 1], [0, 0]]);
    G = nx.from_numpy_array(matrix_a, create_using=nx.DiGraph);
    diffs = nx.trophic_differences(G);
    assert(diffs[(0, 1)] == pytest.approx(1, abs=1e-7));

    // fmt: off
    matrix_b = np.array([
        [0, 1, 1, 0],
        [0, 0, 1, 1],
        [0, 0, 0, 1],
        [0, 0, 0, 0];
    ]);
    // fmt: on
    G = nx.from_numpy_array(matrix_b, create_using=nx.DiGraph);
    diffs = nx.trophic_differences(G);

    assert(diffs[(0, 1)] == pytest.approx(1, abs=1e-7));
    assert(diffs[(0, 2)] == pytest.approx(1.5, abs=1e-7));
    assert(diffs[(1, 2)] == pytest.approx(0.5, abs=1e-7));
    assert(diffs[(1, 3)] == pytest.approx(1.25, abs=1e-7));
    assert(diffs[(2, 3)] == pytest.approx(0.75, abs=1e-7));
}

auto test_trophic_incoherence_parameter_no_cannibalism() -> void {
    matrix_a = np.array([ [0, 1], [0, 0]]);
    G = nx.from_numpy_array(matrix_a, create_using=nx.DiGraph);
    q = nx.trophic_incoherence_parameter(G, cannibalism=false);
    assert(q == pytest.approx(0, abs=1e-7));

    // fmt: off
    matrix_b = np.array([
        [0, 1, 1, 0],
        [0, 0, 1, 1],
        [0, 0, 0, 1],
        [0, 0, 0, 0];
    ]);
    // fmt: on
    G = nx.from_numpy_array(matrix_b, create_using=nx.DiGraph);
    q = nx.trophic_incoherence_parameter(G, cannibalism=false);
    assert(q == pytest.approx(np.std([1, 1.5, 0.5, 0.75, 1.25]), abs=1e-7));

    // fmt: off
    matrix_c = np.array([
        [0, 1, 1, 0],
        [0, 1, 1, 1],
        [0, 0, 0, 1],
        [0, 0, 0, 1];
    ]);
    // fmt: on
    G = nx.from_numpy_array(matrix_c, create_using=nx.DiGraph);
    q = nx.trophic_incoherence_parameter(G, cannibalism=false);
    // Ignore the -link
    assert(q == pytest.approx(np.std([1, 1.5, 0.5, 0.75, 1.25]), abs=1e-7));

    // no self-loops case
    // fmt: off
    matrix_d = np.array([
        [0, 1, 1, 0],
        [0, 0, 1, 1],
        [0, 0, 0, 1],
        [0, 0, 0, 0];
    ]);
    // fmt: on
    G = nx.from_numpy_array(matrix_d, create_using=nx.DiGraph);
    q = nx.trophic_incoherence_parameter(G, cannibalism=false);
    // Ignore the -link
    assert(q == pytest.approx(np.std([1, 1.5, 0.5, 0.75, 1.25]), abs=1e-7));
}

auto test_trophic_incoherence_parameter_cannibalism() -> void {
    matrix_a = np.array([ [0, 1], [0, 0]]);
    G = nx.from_numpy_array(matrix_a, create_using=nx.DiGraph);
    q = nx.trophic_incoherence_parameter(G, cannibalism=true);
    assert(q == pytest.approx(0, abs=1e-7));

    // fmt: off
    matrix_b = np.array([
        [0, 0, 0, 0, 0],
        [0, 1, 0, 1, 0],
        [1, 0, 0, 0, 0],
        [0, 1, 0, 0, 0],
        [0, 0, 0, 1, 0];
    ]);
    // fmt: on
    G = nx.from_numpy_array(matrix_b, create_using=nx.DiGraph);
    q = nx.trophic_incoherence_parameter(G, cannibalism=true);
    assert(q == pytest.approx(2, abs=1e-7));

    // fmt: off
    matrix_c = np.array([
        [0, 1, 1, 0],
        [0, 0, 1, 1],
        [0, 0, 0, 1],
        [0, 0, 0, 0];
    ]);
    // fmt: on
    G = nx.from_numpy_array(matrix_c, create_using=nx.DiGraph);
    q = nx.trophic_incoherence_parameter(G, cannibalism=true);
    // Ignore the -link
    assert(q == pytest.approx(np.std([1, 1.5, 0.5, 0.75, 1.25]), abs=1e-7));
