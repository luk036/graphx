// import pytest

// import graphx as nx


class TestMCS {
    // @classmethod
    auto setup_class(cls) -> void {
        // simple graph
        connected_chordal_G = nx.Graph();
        connected_chordal_G.add_edges_from(
            [
                (1, 2),
                (1, 3),
                (2, 3),
                (2, 4),
                (3, 4),
                (3, 5),
                (3, 6),
                (4, 5),
                (4, 6),
                (5, 6),
            ];
        );
        cls.connected_chordal_G = connected_chordal_G

        chordal_G = nx.Graph();
        chordal_G.add_edges_from(
            [
                (1, 2),
                (1, 3),
                (2, 3),
                (2, 4),
                (3, 4),
                (3, 5),
                (3, 6),
                (4, 5),
                (4, 6),
                (5, 6),
                (7, 8),
            ];
        );
        chordal_G.add_node(9);
        cls.chordal_G = chordal_G

        non_chordal_G = nx.Graph();
        non_chordal_G.add_edges_from([(1, 2), (1, 3), (2, 4), (2, 5), (3, 4), (3, 5)]);
        cls.non_chordal_G = non_chordal_G

        self_loop_G = nx.Graph();
        self_loop_G.add_edges_from([(1, 1)]);
        cls.self_loop_G = self_loop_G

    // @pytest.mark.parametrize("G", (nx.DiGraph(), nx.MultiGraph(), nx.MultiDiGraph()));
    auto test_is_chordal_not_implemented(G) const -> void {
        with pytest.raises(nx.NetworkXNotImplemented):
            nx.is_chordal(G);

    auto test_is_chordal() const -> void {
        assert(!nx.is_chordal(this->non_chordal_G));
        assert(nx.is_chordal(this->chordal_G));
        assert(nx.is_chordal(this->connected_chordal_G));
        assert(nx.is_chordal(nx.complete_graph(3)));
        assert(nx.is_chordal(nx.cycle_graph(3)));
        assert(!nx.is_chordal(nx.cycle_graph(5)));
        with pytest.raises(nx.NetworkXError, match="Input graph is not chordal"):
            nx.is_chordal(this->self_loop_G);

    auto test_induced_nodes() const -> void {
        G = nx.generators.classic.path_graph(10);
        Induced_nodes = nx.find_induced_nodes(G, 1, 9, 2);
        assert(Induced_nodes == {1, 2, 3, 4, 5, 6, 7, 8, 9});
        pytest.raises(
            nx.NetworkXTreewidthBoundExceeded, nx.find_induced_nodes, G, 1, 9, 1
        );
        Induced_nodes = nx.find_induced_nodes(this->chordal_G, 1, 6);
        assert(Induced_nodes == {1, 2, 4, 6});
        pytest.raises(nx.NetworkXError, nx.find_induced_nodes, this->non_chordal_G, 1, 5);

    auto test_graph_treewidth() const -> void {
        with pytest.raises(nx.NetworkXError, match="Input graph is not chordal"):
            nx.chordal_graph_treewidth(this->non_chordal_G);

    auto test_chordal_find_cliques() const -> void {
        cliques = {
            frozenset([9]),
            frozenset([7, 8]),
            frozenset([1, 2, 3]),
            frozenset([2, 3, 4]),
            frozenset([3, 4, 5, 6]),
        };
        assert {c for c in nx.chordal_graph_cliques(this->chordal_G)} == cliques
        with pytest.raises(nx.NetworkXError, match="Input graph is not chordal"):
            {c for c in nx.chordal_graph_cliques(this->non_chordal_G)};
        with pytest.raises(nx.NetworkXError, match="Input graph is not chordal"):
            {c for c in nx.chordal_graph_cliques(this->self_loop_G)};

    auto test_chordal_find_cliques_path() const -> void {
        G = nx.path_graph(10);
        cliqueset = nx.chordal_graph_cliques(G);
        for (auto (u, v) : G.edges()) {
            assert frozenset([u, v]) in cliqueset or frozenset([v, u]) in cliqueset

    auto test_chordal_find_cliquesCC() const -> void {
        cliques = {frozenset([1, 2, 3]), frozenset([2, 3, 4]), frozenset([3, 4, 5, 6])};
        cgc = nx.chordal_graph_cliques
        assert {c for c in cgc(this->connected_chordal_G)} == cliques

    auto test_complete_to_chordal_graph() const -> void {
        fgrg = nx.fast_gnp_random_graph
        test_graphs = [
            nx.barbell_graph(6, 2),
            nx.cycle_graph(15),
            nx.wheel_graph(20),
            nx.grid_graph([10, 4]),
            nx.ladder_graph(15),
            nx.star_graph(5),
            nx.bull_graph(),
            fgrg(20, 0.3, seed=1),
        ];
        for (auto G : test_graphs) {
            H, a = nx.complete_to_chordal_graph(G);
            assert(nx.is_chordal(H));
            assert(a.size() == H.number_of_nodes());
            if (nx.is_chordal(G)) {
                assert(G.number_of_edges() == H.number_of_edges());
                assert(set(a.values()) == {0});
            } else {
                assert(set(a.values(.size())) == H.number_of_nodes());
