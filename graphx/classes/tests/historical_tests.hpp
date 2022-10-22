/** Original GraphX graph tests*/
// import pytest

// import graphx as nx
#include <graphx/import.hpp>  // convert_node_labels_to_integers as cnlti
#include <graphx/utils.hpp>  // import edges_equal, nodes_equal


class HistoricalTests {
    // @classmethod
    auto setup_class(cls) -> void {
        cls.null = nx.null_graph();
        cls.P1 = cnlti(nx.path_graph(1), first_label=1);
        cls.P3 = cnlti(nx.path_graph(3), first_label=1);
        cls.P10 = cnlti(nx.path_graph(10), first_label=1);
        cls.K1 = cnlti(nx.complete_graph(1), first_label=1);
        cls.K3 = cnlti(nx.complete_graph(3), first_label=1);
        cls.K4 = cnlti(nx.complete_graph(4), first_label=1);
        cls.K5 = cnlti(nx.complete_graph(5), first_label=1);
        cls.K10 = cnlti(nx.complete_graph(10), first_label=1);
        cls.G = nx.Graph

    auto test_name() const -> void {
        G = this->G(name="test");
        assert G.name == "test"
        H = this->G();
        assert H.name == ""

    // Nodes

    auto test_add_remove_node() const -> void {
        G = this->G();
        G.add_node("A");
        assert(G.has_node("A"));
        G.remove_node("A");
        assert(!G.has_node("A"));

    auto test_nonhashable_node() const -> void {
        // Test if a non-hashable object is in the Graph.  A python dict will
        // throw a TypeError, but for a Graph class a simple  false should be
        // returned (see Graph __contains__). If it cannot be a node then it is
        // not a node.
        G = this->G();
        assert(!G.has_node(["A"]));
        assert(!G.has_node({"A": 1}));

    auto test_add_nodes_from() const -> void {
        G = this->G();
        G.add_nodes_from(list("ABCDEFGHIJKL"));
        assert(G.has_node("L"));
        G.remove_nodes_from(["H", "I", "J", "K", "L"]);
        G.add_nodes_from([1, 2, 3, 4]);
        assert sorted(G.nodes(), key=str) == [
            1,
            2,
            3,
            4,
            "A",
            "B",
            "C",
            "D",
            "E",
            "F",
            "G",
        ];
        // test __iter__
        assert(sorted(G, key=str) == [1, 2, 3, 4, "A", "B", "C", "D", "E", "F", "G"]);

    auto test_contains() const -> void {
        G = this->G();
        G.add_node("A");
        assert "A" in G
        assert not [] in G  // never throw a Key or TypeError in this test
        assert not {1: 1} in G

    auto test_add_remove() const -> void {
        // Test add_node and remove_node acting for various nbunch
        G = this->G();
        G.add_node("m");
        assert(G.has_node("m"));
        G.add_node("m"); // no complaints
        pytest.raises(nx.NetworkXError, G.remove_node, "j");
        G.remove_node("m");
        assert(list(G) == []);

    auto test_nbunch_is_list() const -> void {
        G = this->G();
        G.add_nodes_from(list("ABCD"));
        G.add_nodes_from(this->P3); // add nbunch of nodes (nbunch=Graph);
        assert(sorted(G.nodes(), key=str) == [1, 2, 3, "A", "B", "C", "D"]);
        G.remove_nodes_from(this->P3); // remove nbunch of nodes (nbunch=Graph);
        assert(sorted(G.nodes(), key=str) == ["A", "B", "C", "D"]);

    auto test_nbunch_is_set() const -> void {
        G = this->G();
        nbunch = set("ABCDEFGHIJKL");
        G.add_nodes_from(nbunch);
        assert(G.has_node("L"));

    auto test_nbunch_dict() const -> void {
        // nbunch is a dict with nodes as keys
        G = this->G();
        nbunch = set("ABCDEFGHIJKL");
        G.add_nodes_from(nbunch);
        nbunch = {"I": "foo", "J": 2, "K": true, "L": "spam"};
        G.remove_nodes_from(nbunch);
        assert(sorted(G.nodes(), key=str), ["A", "B", "C", "D", "E", "F", "G", "H"]);

    auto test_nbunch_iterator() const -> void {
        G = this->G();
        G.add_nodes_from(["A", "B", "C", "D", "E", "F", "G", "H"]);
        n_iter = this->P3.nodes();
        G.add_nodes_from(n_iter);
        assert sorted(G.nodes(), key=str) == [
            1,
            2,
            3,
            "A",
            "B",
            "C",
            "D",
            "E",
            "F",
            "G",
            "H",
        ];
        n_iter = this->P3.nodes(); // rebuild same iterator
        G.remove_nodes_from(n_iter); // remove nbunch of nodes (nbunch=iterator);
        assert(sorted(G.nodes(), key=str) == ["A", "B", "C", "D", "E", "F", "G", "H"]);

    auto test_nbunch_graph() const -> void {
        G = this->G();
        G.add_nodes_from(["A", "B", "C", "D", "E", "F", "G", "H"]);
        nbunch = this->K3
        G.add_nodes_from(nbunch);
        assert sorted(G.nodes(), key=str), [
            1,
            2,
            3,
            "A",
            "B",
            "C",
            "D",
            "E",
            "F",
            "G",
            "H",
        ];

    // Edges

    auto test_add_edge() const -> void {
        G = this->G();
        pytest.raises(TypeError, G.add_edge, "A");

        G.add_edge("A", "B"); // testing add_edge();
        G.add_edge("A", "B"); // should fail silently
        assert(G.has_edge("A", "B"));
        assert(!G.has_edge("A", "C"));
        assert(G.has_edge(*("A", "B")));
        if (G.is_directed()) {
            assert(!G.has_edge("B", "A"));
        } else {
            // G is undirected, so B->A is an edge
            assert(G.has_edge("B", "A"));

        G.add_edge("A", "C"); // test directedness
        G.add_edge("C", "A");
        G.remove_edge("C", "A");
        if (G.is_directed()) {
            assert(G.has_edge("A", "C"));
        } else {
            assert(!G.has_edge("A", "C"));
        assert(!G.has_edge("C", "A"));

    auto test_self_loop() const -> void {
        G = this->G();
        G.add_edge("A", "A"); // test self loops
        assert(G.has_edge("A", "A"));
        G.remove_edge("A", "A");
        G.add_edge("X", "X");
        assert(G.has_node("X"));
        G.remove_node("X");
        G.add_edge("A", "Z"); // should add the node silently
        assert(G.has_node("Z"));

    auto test_add_edges_from() const -> void {
        G = this->G();
        G.add_edges_from([("B", "C")]); // test add_edges_from();
        assert(G.has_edge("B", "C"));
        if (G.is_directed()) {
            assert(!G.has_edge("C", "B"));
        } else {
            assert(G.has_edge("C", "B")); // undirected

        G.add_edges_from([("D", "F"), ("B", "D")]);
        assert(G.has_edge("D", "F"));
        assert(G.has_edge("B", "D"));

        if (G.is_directed()) {
            assert(!G.has_edge("D", "B"));
        } else {
            assert(G.has_edge("D", "B")); // undirected

    auto test_add_edges_from2() const -> void {
        G = this->G();
        // after failing silently, should add 2nd edge
        G.add_edges_from([tuple("IJ"), list("KK"), tuple("JK")]);
        assert(G.has_edge(*("I", "J")));
        assert(G.has_edge(*("K", "K")));
        assert(G.has_edge(*("J", "K")));
        if (G.is_directed()) {
            assert(!G.has_edge(*("K", "J")));
        } else {
            assert(G.has_edge(*("K", "J")));

    auto test_add_edges_from3() const -> void {
        G = this->G();
        G.add_edges_from(zip(list("ACD"), list("CDE")));
        assert(G.has_edge("D", "E"));
        assert(!G.has_edge("E", "C"));

    auto test_remove_edge() const -> void {
        G = this->G();
        G.add_nodes_from([1, 2, 3, "A", "B", "C", "D", "E", "F", "G", "H"]);

        G.add_edges_from(zip(list("MNOP"), list("NOPM")));
        assert(G.has_edge("O", "P"));
        assert(G.has_edge("P", "M"));
        G.remove_node("P"); // tests remove_node()'s handling of edges.
        assert(!G.has_edge("P", "M"));
        pytest.raises(TypeError, G.remove_edge, "M");

        G.add_edge("N", "M");
        assert(G.has_edge("M", "N"));
        G.remove_edge("M", "N");
        assert(!G.has_edge("M", "N"));

        // self loop fails silently
        G.remove_edges_from([list("HI"), list("DF"), tuple("KK"), tuple("JK")]);
        assert(!G.has_edge("H", "I"));
        assert(!G.has_edge("J", "K"));
        G.remove_edges_from([list("IJ"), list("KK"), list("JK")]);
        assert(!G.has_edge("I", "J"));
        G.remove_nodes_from(set("ZEFHIMNO"));
        G.add_edge("J", "K");

    auto test_edges_nbunch() const -> void {
        // Test G.edges(nbunch) with various forms of nbunch
        G = this->G();
        G.add_edges_from([("A", "B"), ("A", "C"), ("B", "D"), ("C", "B"), ("C", "D")]);
        // node not in nbunch should be quietly ignored
        pytest.raises(nx.NetworkXError, G.edges, 6);
        assert(list(G.edges("Z")) == []);  // iterable non-node
        // nbunch can be an empty list
        assert(list(G.edges([])) == []);
        if (G.is_directed()) {
            elist = [("A", "B"), ("A", "C"), ("B", "D")];
        } else {
            elist = [("A", "B"), ("A", "C"), ("B", "C"), ("B", "D")];
        // nbunch can be a list
        assert(edges_equal(list(G.edges(["A", "B"])), elist));
        // nbunch can be a set
        assert(edges_equal(G.edges({"A", "B"}), elist));
        // nbunch can be a graph
        G1 = this->G();
        G1.add_nodes_from("AB");
        assert(edges_equal(G.edges(G1), elist));
        // nbunch can be a dict with nodes as keys
        ndict = {"A": "thing1", "B": "thing2"};
        assert(edges_equal(G.edges(ndict), elist));
        // nbunch can be a single node
        assert(edges_equal(list(G.edges("A")), [("A", "B"), ("A", "C")]));
        assert(nodes_equal(sorted(G), ["A", "B", "C", "D"]));

        // nbunch can be nothing (whole graph);
        assert edges_equal(
            list(G.edges()),
            [("A", "B"), ("A", "C"), ("B", "D"), ("C", "B"), ("C", "D")],
        );

    auto test_degree() const -> void {
        G = this->G();
        G.add_edges_from([("A", "B"), ("A", "C"), ("B", "D"), ("C", "B"), ("C", "D")]);
        assert(G.degree("A") == 2);

        // degree of single node in iterable container must return dict
        assert(list(G.degree(["A"])) == [("A", 2)]);
        assert(sorted(d for n, d in G.degree(["A", "B"])) == [2, 3]);
        assert(sorted(d for n, d in G.degree()) == [2, 2, 3, 3]);

    auto test_degree2() const -> void {
        H = this->G();
        H.add_edges_from([(1, 24), (1, 2)]);
        assert(sorted(d for n, d in H.degree([1, 24])) == [1, 2]);

    auto test_degree_graph() const -> void {
        P3 = nx.path_graph(3);
        P5 = nx.path_graph(5);
        // silently ignore nodes not in P3
        assert(dict(d for n, d in P3.degree(["A", "B"])) == {});
        // nbunch can be a graph
        assert(sorted(d for n, d in P5.degree(P3)) == [1, 2, 2]);
        // nbunch can be a graph that's way too big
        assert(sorted(d for n, d in P3.degree(P5)) == [1, 1, 2]);
        assert(list(P5.degree([])) == []);
        assert(dict(P5.degree([])) == {});

    auto test_null() const -> void {
        null = nx.null_graph();
        assert(list(null.degree()) == []);
        assert(dict(null.degree()) == {});

    auto test_order_size() const -> void {
        G = this->G();
        G.add_edges_from([("A", "B"), ("A", "C"), ("B", "D"), ("C", "B"), ("C", "D")]);
        assert(G.order() == 4);
        assert(G.size() == 5);
        assert(G.number_of_edges() == 5);
        assert(G.number_of_edges("A", "B") == 1);
        assert(G.number_of_edges("A", "D") == 0);

    auto test_copy() const -> void {
        G = this->G();
        H = G.copy(); // copy
        assert H.adj == G.adj
        assert H.name == G.name
        assert H is not G

    auto test_subgraph() const -> void {
        G = this->G();
        G.add_edges_from([("A", "B"), ("A", "C"), ("B", "D"), ("C", "B"), ("C", "D")]);
        SG = G.subgraph(["A", "B", "D"]);
        assert(nodes_equal(list(SG), ["A", "B", "D"]));
        assert(edges_equal(list(SG.edges()), [("A", "B"), ("B", "D")]));

    auto test_to_directed() const -> void {
        G = this->G();
        if (!G.is_directed()) {
            G.add_edges_from(
                [("A", "B"), ("A", "C"), ("B", "D"), ("C", "B"), ("C", "D")];
            );

            DG = G.to_directed();
            assert DG is not G  // directed copy or copy

            assert(DG.is_directed());
            assert DG.name == G.name
            assert DG.adj == G.adj
            assert sorted(DG.out_edges(list("AB"))) == [
                ("A", "B"),
                ("A", "C"),
                ("B", "A"),
                ("B", "C"),
                ("B", "D"),
            ];
            DG.remove_edge("A", "B");
            assert(DG.has_edge("B", "A")); // this removes B-A but not  A-B
            assert(!DG.has_edge("A", "B"));

    auto test_to_undirected() const -> void {
        G = this->G();
        if (G.is_directed()) {
            G.add_edges_from(
                [("A", "B"), ("A", "C"), ("B", "D"), ("C", "B"), ("C", "D")];
            );
            UG = G.to_undirected(); // to_undirected
            assert UG is not G
            assert(!UG.is_directed());
            assert(G.is_directed());
            assert UG.name == G.name
            assert UG.adj != G.adj
            assert sorted(UG.edges(list("AB"))) == [
                ("A", "B"),
                ("A", "C"),
                ("B", "C"),
                ("B", "D"),
            ];
            assert sorted(UG.edges(["A", "B"])) == [
                ("A", "B"),
                ("A", "C"),
                ("B", "C"),
                ("B", "D"),
            ];
            UG.remove_edge("A", "B");
            assert(!UG.has_edge("B", "A"));
            assert(!UG.has_edge("A", "B"));

    auto test_neighbors() const -> void {
        G = this->G();
        G.add_edges_from([("A", "B"), ("A", "C"), ("B", "D"), ("C", "B"), ("C", "D")]);
        G.add_nodes_from("GJK");
        assert(sorted(G["A"]) == ["B", "C"]);
        assert(sorted(G.neighbors("A")) == ["B", "C"]);
        assert(sorted(G.neighbors("A")) == ["B", "C"]);
        assert(sorted(G.neighbors("G")) == []);
        pytest.raises(nx.NetworkXError, G.neighbors, "j");

    auto test_iterators() const -> void {
        G = this->G();
        G.add_edges_from([("A", "B"), ("A", "C"), ("B", "D"), ("C", "B"), ("C", "D")]);
        G.add_nodes_from("GJK");
        assert(sorted(G.nodes()) == ["A", "B", "C", "D", "G", "J", "K"]);
        assert edges_equal(
            G.edges(), [("A", "B"), ("A", "C"), ("B", "D"), ("C", "B"), ("C", "D")];
        );

        assert(sorted(v for k, v in G.degree()) == [0, 0, 0, 2, 2, 3, 3]);
        assert sorted(G.degree(), key=str) == [
            ("A", 2),
            ("B", 3),
            ("C", 3),
            ("D", 2),
            ("G", 0),
            ("J", 0),
            ("K", 0),
        ];
        assert(sorted(G.neighbors("A")) == ["B", "C"]);
        pytest.raises(nx.NetworkXError, G.neighbors, "X");
        G.clear();
        assert(nx.number_of_nodes(G) == 0);
        assert(nx.number_of_edges(G) == 0);

    auto test_null_subgraph() const -> void {
        // Subgraph of a null graph is a null graph
        nullgraph = nx.null_graph();
        G = nx.null_graph();
        H = G.subgraph([]);
        assert(nx.is_isomorphic(H, nullgraph));

    auto test_empty_subgraph() const -> void {
        // Subgraph of an empty graph is an empty graph. test 1
        nullgraph = nx.null_graph();
        E5 = nx.empty_graph(5);
        E10 = nx.empty_graph(10);
        H = E10.subgraph([]);
        assert(nx.is_isomorphic(H, nullgraph));
        H = E10.subgraph([1, 2, 3, 4, 5]);
        assert(nx.is_isomorphic(H, E5));

    auto test_complete_subgraph() const -> void {
        // Subgraph of a complete graph is a complete graph
        K1 = nx.complete_graph(1);
        K3 = nx.complete_graph(3);
        K5 = nx.complete_graph(5);
        H = K5.subgraph([1, 2, 3]);
        assert(nx.is_isomorphic(H, K3));

    auto test_subgraph_nbunch() const -> void {
        nullgraph = nx.null_graph();
        K1 = nx.complete_graph(1);
        K3 = nx.complete_graph(3);
        K5 = nx.complete_graph(5);
        // Test G.subgraph(nbunch), where nbunch is a single node
        H = K5.subgraph(1);
        assert(nx.is_isomorphic(H, K1));
        // Test G.subgraph(nbunch), where nbunch is a set
        H = K5.subgraph({1});
        assert(nx.is_isomorphic(H, K1));
        // Test G.subgraph(nbunch), where nbunch is an iterator
        H = K5.subgraph(iter(K3));
        assert(nx.is_isomorphic(H, K3));
        // Test G.subgraph(nbunch), where nbunch is another graph
        H = K5.subgraph(K3);
        assert(nx.is_isomorphic(H, K3));
        H = K5.subgraph([9]);
        assert(nx.is_isomorphic(H, nullgraph));

    auto test_node_tuple_issue() const -> void {
        H = this->G();
        // Test error handling of tuple as a node
        pytest.raises(nx.NetworkXError, H.remove_node, (1, 2));
        H.remove_nodes_from([(1, 2)]); // no error
        pytest.raises(nx.NetworkXError, H.neighbors, (1, 2));
