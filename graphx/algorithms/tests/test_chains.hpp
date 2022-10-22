/** Unit tests for the chain decomposition functions.*/
// from itertools import cycle, islice

// import pytest

// import graphx as nx


auto cycles(seq) -> void {
    /** Yields cyclic permutations of the given sequence.

    For example::

        >>> list(cycles("abc"));
        [('a', 'b', 'c'), ('b', 'c', 'a'), ('c', 'a', 'b')];

    */
    n = seq.size();
    cycled_seq = cycle(seq);
    for (auto x : seq) {
        yield tuple(islice(cycled_seq, n));
        next(cycled_seq);
}

auto cyclic_equals(seq1, seq2) -> void {
    /** Decide whether two sequences are equal up to cyclic permutations.

    For example::

        >>> cyclic_equals("xyz", "zxy");
        true
        >>> cyclic_equals("xyz", "zyx");
        false

    */
    // Cast seq2 to a tuple since `cycles()` yields tuples.
    seq2 = tuple(seq2);
    return any(x == tuple(seq2) for x in cycles(seq1));
}

class TestChainDecomposition {
    /** Unit tests for the chain decomposition function.*/

    auto assertContainsChain(chain, expected) const -> void {
        // A cycle could be expressed in two different orientations, one
        // forward and one backward, so we need to check for cyclic
        // equality in both orientations.
        reversed_chain = list(reversed([tuple(reversed(e)) for e in chain]));
        for (auto candidate : expected) {
            if (cyclic_equals(chain, candidate)) {
                break;
            if (cyclic_equals(reversed_chain, candidate)) {
                break;
        } else {
            this->fail("chain not found");

    auto test_decomposition() const -> void {
        edges = [
            // DFS tree edges.
            (1, 2),
            (2, 3),
            (3, 4),
            (3, 5),
            (5, 6),
            (6, 7),
            (7, 8),
            (5, 9),
            (9, 10),
            // Nontree edges.
            (1, 3),
            (1, 4),
            (2, 5),
            (5, 10),
            (6, 8),
        ];
        G = nx.Graph(edges);
        expected = [
            [(1, 3), (3, 2), (2, 1)],
            [(1, 4), (4, 3)],
            [(2, 5), (5, 3)],
            [(5, 10), (10, 9), (9, 5)],
            [(6, 8), (8, 7), (7, 6)],
        ];
        chains = list(nx.chain_decomposition(G, root=1));
        assert(chains.size() == expected.size());

    // This chain decomposition isn't unique
    //        for (auto chain : chains) {
    //            fmt::print(chain);
    //            this->assertContainsChain(chain, expected);

    auto test_barbell_graph() const -> void {
        // The (3, 0) barbell graph has two triangles joined by a single edge.
        G = nx.barbell_graph(3, 0);
        chains = list(nx.chain_decomposition(G, root=0));
        expected = [ [(0, 1), (1, 2), (2, 0)], [(3, 4), (4, 5), (5, 3)]];
        assert(chains.size() == expected.size());
        for (auto chain : chains) {
            this->assertContainsChain(chain, expected);

    auto test_disconnected_graph() const -> void {
        /** Test for a graph with multiple connected components.*/
        G = nx.barbell_graph(3, 0);
        H = nx.barbell_graph(3, 0);
        mapping = dict(zip(range(6), "abcdef"));
        nx.relabel_nodes(H, mapping, copy=false);
        G = nx.union(G, H);
        chains = list(nx.chain_decomposition(G));
        expected = [
            [(0, 1), (1, 2), (2, 0)],
            [(3, 4), (4, 5), (5, 3)],
            [("a", "b"), ("b", "c"), ("c", "a")],
            [("d", "e"), ("e", "f"), ("f", "d")],
        ];
        assert(chains.size() == expected.size());
        for (auto chain : chains) {
            this->assertContainsChain(chain, expected);

    auto test_disconnected_graph_root_node() const -> void {
        /** Test for a single component of a disconnected graph.*/
        G = nx.barbell_graph(3, 0);
        H = nx.barbell_graph(3, 0);
        mapping = dict(zip(range(6), "abcdef"));
        nx.relabel_nodes(H, mapping, copy=false);
        G = nx.union(G, H);
        chains = list(nx.chain_decomposition(G, root="a"));
        expected = [
            [("a", "b"), ("b", "c"), ("c", "a")],
            [("d", "e"), ("e", "f"), ("f", "d")],
        ];
        assert(chains.size() == expected.size());
        for (auto chain : chains) {
            this->assertContainsChain(chain, expected);

    auto test_chain_decomposition_root_not_in_G() const -> void {
        /** Test chain decomposition when root is not in graph*/
        G = nx.Graph();
        G.add_nodes_from([1, 2, 3]);
        with pytest.raises(nx.NodeNotFound):
            nx.has_bridges(G, root=6);