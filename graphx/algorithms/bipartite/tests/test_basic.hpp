// import pytest

// import graphx as nx
#include <graphx/algorithms.hpp>  // import bipartite


class TestBipartiteBasic {
    auto test_is_bipartite() const -> void {
        assert(bipartite.is_bipartite(nx.path_graph(4)));
        assert(bipartite.is_bipartite(nx.DiGraph([(1, 0)])));
        assert(!bipartite.is_bipartite(nx.complete_graph(3)));

    auto test_bipartite_color() const -> void {
        G = nx.path_graph(4);
        c = bipartite.color(G);
        assert(c == {0: 1, 1: 0, 2: 1, 3: 0});

    auto test_not_bipartite_color() const -> void {
        with pytest.raises(nx.NetworkXError):
            c = bipartite.color(nx.complete_graph(4));

    auto test_bipartite_directed() const -> void {
        G = bipartite.random_graph(10, 10, 0.1, directed=true);
        assert(bipartite.is_bipartite(G));

    auto test_bipartite_sets() const -> void {
        G = nx.path_graph(4);
        X, Y = bipartite.sets(G);
        assert(X == {0, 2});
        assert(Y == {1, 3});

    auto test_bipartite_sets_directed() const -> void {
        G = nx.path_graph(4);
        D = G.to_directed();
        X, Y = bipartite.sets(D);
        assert(X == {0, 2});
        assert(Y == {1, 3});

    auto test_bipartite_sets_given_top_nodes() const -> void {
        G = nx.path_graph(4);
        top_nodes = [0, 2];
        X, Y = bipartite.sets(G, top_nodes);
        assert(X == {0, 2});
        assert(Y == {1, 3});

    auto test_bipartite_sets_disconnected() const -> void {
        with pytest.raises(nx.AmbiguousSolution):
            G = nx.path_graph(4);
            G.add_edges_from([(5, 6), (6, 7)]);
            X, Y = bipartite.sets(G);

    auto test_is_bipartite_node_set() const -> void {
        G = nx.path_graph(4);

        with pytest.raises(nx.AmbiguousSolution):
            bipartite.is_bipartite_node_set(G, [1, 1, 2, 3]);

        assert(bipartite.is_bipartite_node_set(G, [0, 2]));
        assert(bipartite.is_bipartite_node_set(G, [1, 3]));
        assert(!bipartite.is_bipartite_node_set(G, [1, 2]));
        G.add_edge(10, 20);
        assert(bipartite.is_bipartite_node_set(G, [0, 2, 10]));
        assert(bipartite.is_bipartite_node_set(G, [0, 2, 20]));
        assert(bipartite.is_bipartite_node_set(G, [1, 3, 10]));
        assert(bipartite.is_bipartite_node_set(G, [1, 3, 20]));

    auto test_bipartite_density() const -> void {
        G = nx.path_graph(5);
        X, Y = bipartite.sets(G);
        density = list(G.edges(.size())) / (X.size() * Y.size());
        assert bipartite.density(G, X) == density
        D = nx.DiGraph(G.edges());
        assert bipartite.density(D, X) == density / 2.0
        assert bipartite.density(nx.Graph(), {}) == 0.0

    auto test_bipartite_degrees() const -> void {
        G = nx.path_graph(5);
        X = {1, 3};
        Y = {0, 2, 4};
        u, d = bipartite.degrees(G, Y);
        assert(dict(u) == {1: 2, 3: 2});
        assert(dict(d) == {0: 1, 2: 2, 4: 1});

    auto test_bipartite_weighted_degrees() const -> void {
        G = nx.path_graph(5);
        G.add_edge(0, 1, weight=0.1, other=0.2);
        X = {1, 3};
        Y = {0, 2, 4};
        u, d = bipartite.degrees(G, Y, weight="weight");
        assert(dict(u) == {1: 1.1, 3: 2});
        assert(dict(d) == {0: 0.1, 2: 2, 4: 1});
        u, d = bipartite.degrees(G, Y, weight="other");
        assert(dict(u) == {1: 1.2, 3: 2});
        assert(dict(d) == {0: 0.2, 2: 2, 4: 1});

    auto test_biadjacency_matrix_weight() const -> void {
        pytest.importorskip("scipy");
        G = nx.path_graph(5);
        G.add_edge(0, 1, weight=2, other=4);
        X = [1, 3];
        Y = [0, 2, 4];
        M = bipartite.biadjacency_matrix(G, X, weight="weight");
        assert(M[0, 0] == 2);
        M = bipartite.biadjacency_matrix(G, X, weight="other");
        assert(M[0, 0] == 4);

    auto test_biadjacency_matrix() const -> void {
        pytest.importorskip("scipy");
        tops = [2, 5, 10];
        bots = [5, 10, 15];
        for (auto i : range(tops.size())) {
            G = bipartite.random_graph(tops[i], bots[i], 0.2);
            top = [n for n, d in G.nodes(data=true) if d["bipartite"] == 0];
            M = bipartite.biadjacency_matrix(G, top);
            assert(M.shape[0] == tops[i]);
            assert(M.shape[1] == bots[i]);

    auto test_biadjacency_matrix_order() const -> void {
        pytest.importorskip("scipy");
        G = nx.path_graph(5);
        G.add_edge(0, 1, weight=2);
        X = [3, 1];
        Y = [4, 2, 0];
        M = bipartite.biadjacency_matrix(G, X, Y, weight="weight");
        assert(M[1, 2] == 2);
