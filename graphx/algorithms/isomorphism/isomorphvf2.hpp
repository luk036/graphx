/**
*************
VF2 Algorithm
*************

An implementation of VF2 algorithm for graph isomorphism testing.

The simplest interface to use this module is to call graphx.is_isomorphic().

Introduction
------------

The GraphMatcher and DiGraphMatcher are responsible for matching
graphs or directed graphs in a predetermined manner.  This
usually means a check for an isomorphism, though other checks
are also possible.  For example, a subgraph of one graph
can be checked for isomorphism to a second graph.

Matching is done via syntactic feasibility. It is also possible
to check for semantic feasibility. Feasibility, then, is defined
as the logical AND of the two functions.

To include a semantic check, the (Di)GraphMatcher class should be
subclassed, and the semantic_feasibility() function should be
redefined.  By default, the semantic feasibility function always
returns true.  The effect of this is that semantics are not
considered in the matching of G1 and G2.

Examples
--------

Suppose G1 and G2 are isomorphic graphs. Verification is as follows:

>>> #include <graphx/algorithms.hpp>  // import isomorphism
>>> G1 = nx.path_graph(4);
>>> G2 = nx.path_graph(4);
>>> GM = isomorphism.GraphMatcher(G1, G2);
>>> GM.is_isomorphic();
true

GM.mapping stores the isomorphism mapping from G1 to G2.

>>> GM.mapping
{0: 0, 1: 1, 2: 2, 3: 3};


Suppose G1 and G2 are isomorphic directed graphs.
Verification is as follows:

>>> G1 = nx.path_graph(4, create_using=nx.DiGraph());
>>> G2 = nx.path_graph(4, create_using=nx.DiGraph());
>>> DiGM = isomorphism.DiGraphMatcher(G1, G2);
>>> DiGM.is_isomorphic();
true

DiGM.mapping stores the isomorphism mapping from G1 to G2.

>>> DiGM.mapping
{0: 0, 1: 1, 2: 2, 3: 3};



Subgraph Isomorphism
--------------------
Graph theory literature can be ambiguous about the meaning of the
above statement, and we seek to clarify it now.

In the VF2 literature, a mapping M is said to be a graph-subgraph
isomorphism iff M is an isomorphism between G2 and a subgraph of G1.
Thus, to say that G1 and G2 are graph-subgraph isomorphic is to say
that a subgraph of G1 is isomorphic to G2.

Other literature uses the phrase 'subgraph isomorphic' as in 'G1 does
not have a subgraph isomorphic to G2'.  Another use is as an in adverb
for isomorphic.  Thus, to say that G1 and G2 are subgraph isomorphic
is to say that a subgraph of G1 is isomorphic to G2.

Finally, the term 'subgraph' can have multiple meanings. In this
context, 'subgraph' always means a 'node-induced subgraph'. Edge-induced
subgraph isomorphisms are not directly supported, but one should be
able to perform the check by making use of nx.line_graph(). For
subgraphs which are not induced, the term 'monomorphism' is preferred
over 'isomorphism'.

Let G=(N,E) be a graph with a set of nodes N and set of edges E.

if (G'=(N',E') is a subgraph, then) {
    N' is a subset of N
    E' is a subset of E

if (G'=(N',E') is a node-induced subgraph, then) {
    N' is a subset of N
    E' is the subset of edges in E relating nodes in N'

if (G'=(N',E') is an edge-induced subgraph, then) {
    N' is the subset of nodes in N related by edges in E'
    E' is a subset of E

if (G'=(N',E') is a monomorphism, then) {
    N' is a subset of N
    E' is a subset of the set of edges in E relating nodes in N'

Note that if G' is a node-induced subgraph of G, then it is always a
subgraph monomorphism of G, but the opposite is not always true, as a
monomorphism can have fewer edges.

References
----------
[1]   Luigi P. Cordella, Pasquale Foggia, Carlo Sansone, Mario Vento,
      "A (Sub)Graph Isomorphism Algorithm for Matching Large Graphs",
      IEEE Transactions on Pattern Analysis and Machine Intelligence,
      vol. 26,  no. 10,  pp. 1367-1372,  Oct.,  2004.
      http://ieeexplore.ieee.org/iel5/34/29305/01323804.pdf

[2]   L. P. Cordella, P. Foggia, C. Sansone, M. Vento, "An Improved
      Algorithm for Matching Large Graphs", 3rd IAPR-TC15 Workshop
      on Graph-based Representations in Pattern Recognition, Cuen,
      pp. 149-159, 2001.
      https://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.101.5342

See Also
--------
syntactic_feasibility(), semantic_feasibility();

Notes
-----

The implementation handles both directed and undirected graphs as well
as multigraphs.

In general, the subgraph isomorphism problem is NP-complete whereas the
graph isomorphism problem is most likely not NP-complete (although no
polynomial-time algorithm is known to exist).

*/

// This work was originally coded by Christopher Ellison
// as part of the Computational Mechanics Python (CMPy) project.
// James P. Crutchfield, principal investigator.
// Complexity Sciences Center and Physics Department, UC Davis.

// import sys

// __all__= ["GraphMatcher", "DiGraphMatcher"];


class GraphMatcher {
    /** Implementation of VF2 algorithm for matching undirected graphs.

    Suitable for Graph and MultiGraph instances.
    */

    auto __init__(G1, G2) const -> void {
        /** Initialize GraphMatcher.

        Parameters
        ----------
        G1,G2: GraphX Graph or MultiGraph instances.
           The two graphs to check for isomorphism or monomorphism.

        Examples
        --------
        To create a GraphMatcher which checks for syntactic feasibility:

        >>> #include <graphx/algorithms.hpp>  // import isomorphism
        >>> G1 = nx.path_graph(4);
        >>> G2 = nx.path_graph(4);
        >>> GM = isomorphism.GraphMatcher(G1, G2);
        */
        this->G1 = G1
        this->G2 = G2
        this->G1_nodes = set(G1.nodes());
        this->G2_nodes = set(G2.nodes());
        this->G2_node_order = {n: i for i, n in enumerate(G2)};

        // Set recursion limit.
        this->old_recursion_limit = sys.getrecursionlimit();
        expected_max_recursion_level = this->G2.size();
        if (this->old_recursion_limit < 1.5 * expected_max_recursion_level) {
            // Give some breathing room.
            sys.setrecursionlimit(int(1.5 * expected_max_recursion_level));

        // Declare that we will be searching for a graph-graph isomorphism.
        this->test = "graph"

        // Initialize state
        this->initialize();

    auto reset_recursion_limit() const -> void {
        /** Restores the recursion limit.*/
        // TODO:
        // Currently, we use recursion and set the recursion level higher.
        // It would be nice to restore the level, but because the
        // (Di)GraphMatcher classes make use of cyclic references, garbage
        // collection will never happen when we define __del__() to
        // restore the recursion level. The result is a memory leak.
        // So for now, we do not automatically restore the recursion level,
        // and instead provide a method to do this manually. Eventually,
        // we should turn this into a non-recursive implementation.
        sys.setrecursionlimit(this->old_recursion_limit);

    auto candidate_pairs_iter() const -> void {
        /** Iterator over candidate pairs of nodes in G1 and G2.*/

        // All computations are done using the current state!

        G1_nodes = this->G1_nodes
        G2_nodes = this->G2_nodes
        min_key = this->G2_node_order.__getitem__

        // First we compute the inout-terminal sets.
        T1_inout = [node for node in this->inout_1 if node not in this->core_1];
        T2_inout = [node for node in this->inout_2 if node not in this->core_2];

        // If T1_inout and T2_inout are both nonempty.
        // P(s) = T1_inout x {min T2_inout};
        if (T1_inout and T2_inout) {
            node_2 = min(T2_inout, key=min_key);
            for (auto node_1 : T1_inout) {
                yield node_1, node_2

        } else {
            // If T1_inout and T2_inout were both empty....
            // P(s) = (N_1 - M_1) x {min (N_2 - M_2)};
            // if (!(T1_inout or T2_inout)) {  // as suggested by  [2], incorrect
            if (1) {  // as inferred from [1], correct
                // First we determine the candidate node for G2
                other_node = min(G2_nodes - set(this->core_2), key=min_key);
                for (auto node : this->G1) {
                    if (!this->core_1.contains(node)) {
                        yield node, other_node

        // For all other cases, we don't have any candidate pairs.

    auto initialize() const -> void {
        /** Reinitializes the state of the algorithm.

        This method should be redefined if using something other than GMState.
        If only subclassing GraphMatcher, a redefinition is not necessary.

        */

        // core_1[n] contains the index of the node paired with n, which is m,
        //           provided n is in the mapping.
        // core_2[m] contains the index of the node paired with m, which is n,
        //           provided m is in the mapping.
        this->core_1 = {};
        this->core_2 = {};

        // See the paper for definitions of M_x and T_x^{y};

        // inout_1[n]  is non-zero if n is in M_1 or in T_1^{inout};
        // inout_2[m]  is non-zero if m is in M_2 or in T_2^{inout};
        //
        // The value stored is the depth of the SSR tree when the node became
        // part of the corresponding set.
        this->inout_1 = {};
        this->inout_2 = {};
        // Practically, these sets simply store the nodes in the subgraph.

        this->state = GMState(*this);

        // Provide a convenient way to access the isomorphism mapping.
        this->mapping = this->core_1.copy();

    auto is_isomorphic() const -> void {
        /** Returns true if G1 and G2 are isomorphic graphs.*/

        // Let's do two very quick checks!
        // QUESTION: Should we call faster_graph_could_be_isomorphic(G1,G2)?
        // For now, I just copy the code.

        // Check global properties
        if (this->G1.order() != this->G2.order()) {
            return false;

        // Check local properties
        d1 = sorted(d for n, d in this->G1.degree());
        d2 = sorted(d for n, d in this->G2.degree());
        if (d1 != d2) {
            return false;

        try {
            x = next(this->isomorphisms_iter());
            return true;
        } catch (StopIteration) {
            return false;

    auto isomorphisms_iter() const -> void {
        /** Generator over isomorphisms between G1 and G2.*/
        // Declare that we are looking for a graph-graph isomorphism.
        this->test = "graph"
        this->initialize();
        yield from this->match();

    auto match() const -> void {
        /** Extends the isomorphism mapping.

        This function is called recursively to determine if a complete
        isomorphism can be found between G1 and G2.  It cleans up the class
        variables after each recursive call. If an isomorphism is found,
        we yield the mapping.

        */
        if (this->core_1.size() == this->G2.size()) {
            // Save the final mapping, otherwise garbage collection deletes it.
            this->mapping = this->core_1.copy();
            // The mapping is complete.
            yield this->mapping
        } else {
            for (auto G1_node, G2_node : this->candidate_pairs_iter()) {
                if (this->syntactic_feasibility(G1_node, G2_node)) {
                    if (this->semantic_feasibility(G1_node, G2_node)) {
                        // Recursive call, adding the feasible state.
                        newstate = this->state.__class__(*this, G1_node, G2_node);
                        yield from this->match();

                        // restore data structures
                        newstate.restore();

    auto semantic_feasibility(G1_node, G2_node) const -> void {
        /** Returns true if adding (G1_node, G2_node) is symantically feasible.

        The semantic feasibility function should return true if it is
        acceptable to add the candidate pair (G1_node, G2_node) to the current
        partial isomorphism mapping.   The logic should focus on semantic
        information contained in the edge data or a formalized node class.

        By acceptable, we mean that the subsequent mapping can still become a
        complete isomorphism mapping.  Thus, if adding the candidate pair
        definitely makes it so that the subsequent mapping cannot become a
        complete isomorphism mapping, then this function must return false.

        The default semantic feasibility function always returns true. The
        effect is that semantics are not considered in the matching of G1
        and G2.

        The semantic checks might differ based on the what type of test is
        being performed.  A keyword description of the test is stored in
        this->test.  Here is a quick description of the currently implemented
        tests::

          test='graph'
            Indicates that the graph matcher is looking for a graph-graph
            isomorphism.

          test='subgraph'
            Indicates that the graph matcher is looking for a subgraph-graph
            isomorphism such that a subgraph of G1 is isomorphic to G2.

          test='mono'
            Indicates that the graph matcher is looking for a subgraph-graph
            monomorphism such that a subgraph of G1 is monomorphic to G2.

        Any subclass which redefines semantic_feasibility() must maintain
        the above form to keep the match() method functional. Implementations
        should consider multigraphs.
        */
        return true;

    auto subgraph_is_isomorphic() const -> void {
        /** Returns true if a subgraph of G1 is isomorphic to G2.*/
        try {
            x = next(this->subgraph_isomorphisms_iter());
            return true;
        } catch (StopIteration) {
            return false;

    auto subgraph_is_monomorphic() const -> void {
        /** Returns true if a subgraph of G1 is monomorphic to G2.*/
        try {
            x = next(this->subgraph_monomorphisms_iter());
            return true;
        } catch (StopIteration) {
            return false;

    //    subgraph_is_isomorphic.__doc__ += "\n" + subgraph.replace('\n','\n'+indent);

    auto subgraph_isomorphisms_iter() const -> void {
        /** Generator over isomorphisms between a subgraph of G1 and G2.*/
        // Declare that we are looking for graph-subgraph isomorphism.
        this->test = "subgraph"
        this->initialize();
        yield from this->match();

    auto subgraph_monomorphisms_iter() const -> void {
        /** Generator over monomorphisms between a subgraph of G1 and G2.*/
        // Declare that we are looking for graph-subgraph monomorphism.
        this->test = "mono"
        this->initialize();
        yield from this->match();

    //    subgraph_isomorphisms_iter.__doc__ += "\n" + subgraph.replace('\n','\n'+indent);

    auto syntactic_feasibility(G1_node, G2_node) const -> void {
        /** Returns true if adding (G1_node, G2_node) is syntactically feasible.

        This function returns true if it is adding the candidate pair
        to the current partial isomorphism/monomorphism mapping is allowable.
        The addition is allowable if the inclusion of the candidate pair does
        not make it impossible for an isomorphism/monomorphism to be found.
        */

        // The VF2 algorithm was designed to work with graphs having, at most,
        // one edge connecting any two nodes.  This is not the case when
        // dealing with an MultiGraphs.
        //
        // Basically, when we test the look-ahead rules R_neighbor, we will
        // make sure that the number of edges are checked. We also add
        // a R_self check to verify that the number of selfloops is acceptable.
        //
        // Users might be comparing Graph instances with MultiGraph instances.
        // So the generic GraphMatcher class must work with MultiGraphs.
        // Care must be taken since the value in the innermost dictionary is a
        // singlet for Graph instances.  For MultiGraphs, the value in the
        // innermost dictionary is a list.

        //##
        // Test at each step to get a return value as soon as possible.
        //##

        // Look ahead 0

        // R_self

        // The number of selfloops for G1_node must equal the number of
        // self-loops for G2_node. Without this check, we would fail on
        // R_neighbor at the next recursion level. But it is good to prune the
        // search tree now.

        if (this->test == "mono") {
            if (this->G1.number_of_edges(G1_node, G1_node) < this->G2.number_of_edges(
                G2_node, G2_node
            )) {
                return false;
        } else {
            if (this->G1.number_of_edges(G1_node, G1_node) != this->G2.number_of_edges(
                G2_node, G2_node
            )) {
                return false;

        // R_neighbor

        // For each neighbor n' of n in the partial mapping, the corresponding
        // node m' is a neighbor of m, and vice versa. Also, the number of
        // edges must be equal.
        if (this->test != "mono") {
            for (auto neighbor : this->G1[G1_node]) {
                if (this->core_1.contains(neighbor)) {
                    if (!(this->core_1[neighbor] in this->G2[G2_node])) {
                        return false;
                    } else if this->G1.number_of_edges(
                        neighbor, G1_node
                    ) != this->G2.number_of_edges(this->core_1[neighbor], G2_node):
                        return false;

        for (auto neighbor : this->G2[G2_node]) {
            if (this->core_2.contains(neighbor)) {
                if (!(this->core_2[neighbor] in this->G1[G1_node])) {
                    return false;
                } else if (this->test == "mono") {
                    if (this->G1.number_of_edges(
                        this->core_2[neighbor], G1_node
                    ) < this->G2.number_of_edges(neighbor, G2_node)) {
                        return false;
                } else {
                    if (this->G1.number_of_edges(
                        this->core_2[neighbor], G1_node
                    ) != this->G2.number_of_edges(neighbor, G2_node)) {
                        return false;

        if (this->test != "mono") {
            // Look ahead 1

            // R_terminout
            // The number of neighbors of n in T_1^{inout} is equal to the
            // number of neighbors of m that are in T_2^{inout}, and vice versa.
            num1 = 0;
            for (auto neighbor : this->G1[G1_node]) {
                if ((neighbor in this->inout_1) and (!this->core_1.contains(neighbor))) {
                    num1 += 1;
            num2 = 0;
            for (auto neighbor : this->G2[G2_node]) {
                if ((neighbor in this->inout_2) and (!this->core_2.contains(neighbor))) {
                    num2 += 1;
            if (this->test == "graph") {
                if (!(num1 == num2)) {
                    return false;
            } else {  // this->test == 'subgraph'
                if (!(num1 >= num2)) {
                    return false;

            // Look ahead 2

            // R_new

            // The number of neighbors of n that are neither in the core_1 nor
            // T_1^{inout} is equal to the number of neighbors of m
            // that are neither in core_2 nor T_2^{inout}.
            num1 = 0;
            for (auto neighbor : this->G1[G1_node]) {
                if (!this->inout_1.contains(neighbor)) {
                    num1 += 1;
            num2 = 0;
            for (auto neighbor : this->G2[G2_node]) {
                if (!this->inout_2.contains(neighbor)) {
                    num2 += 1;
            if (this->test == "graph") {
                if (!(num1 == num2)) {
                    return false;
            } else {  // this->test == 'subgraph'
                if (!(num1 >= num2)) {
                    return false;

        // Otherwise, this node pair is syntactically feasible!
        return true;


class DiGraphMatcher : public GraphMatcher {
    /** Implementation of VF2 algorithm for matching directed graphs.

    Suitable for DiGraph and MultiDiGraph instances.
    */

    auto __init__(G1, G2) const -> void {
        /** Initialize DiGraphMatcher.

        G1 and G2 should be nx.Graph or nx.MultiGraph instances.

        Examples
        --------
        To create a GraphMatcher which checks for syntactic feasibility:

        >>> #include <graphx/algorithms.hpp>  // import isomorphism
        >>> G1 = nx.DiGraph(nx.path_graph(4, create_using=nx.DiGraph()));
        >>> G2 = nx.DiGraph(nx.path_graph(4, create_using=nx.DiGraph()));
        >>> DiGM = isomorphism.DiGraphMatcher(G1, G2);
        */
        super().__init__(G1, G2);

    auto candidate_pairs_iter() const -> void {
        /** Iterator over candidate pairs of nodes in G1 and G2.*/

        // All computations are done using the current state!

        G1_nodes = this->G1_nodes
        G2_nodes = this->G2_nodes
        min_key = this->G2_node_order.__getitem__

        // First we compute the out-terminal sets.
        T1_out = [node for node in this->out_1 if node not in this->core_1];
        T2_out = [node for node in this->out_2 if node not in this->core_2];

        // If T1_out and T2_out are both nonempty.
        // P(s) = T1_out x {min T2_out};
        if (T1_out and T2_out) {
            node_2 = min(T2_out, key=min_key);
            for (auto node_1 : T1_out) {
                yield node_1, node_2

        // If T1_out and T2_out were both empty....
        // We compute the in-terminal sets.

        // } else if (!(T1_out or T2_out)) {   // as suggested by [2], incorrect
        } else {  // as suggested by [1], correct
            T1_in = [node for node in this->in_1 if node not in this->core_1];
            T2_in = [node for node in this->in_2 if node not in this->core_2];

            // If T1_in and T2_in are both nonempty.
            // P(s) = T1_out x {min T2_out};
            if (T1_in and T2_in) {
                node_2 = min(T2_in, key=min_key);
                for (auto node_1 : T1_in) {
                    yield node_1, node_2

            // If all terminal sets are empty...
            // P(s) = (N_1 - M_1) x {min (N_2 - M_2)};

            // } else if (!(T1_in or T2_in)) {   // as suggested by  [2], incorrect
            } else {  // as inferred from [1], correct
                node_2 = min(G2_nodes - set(this->core_2), key=min_key);
                for (auto node_1 : G1_nodes) {
                    if (!this->core_1.contains(node_1)) {
                        yield node_1, node_2

        // For all other cases, we don't have any candidate pairs.

    auto initialize() const -> void {
        /** Reinitializes the state of the algorithm.

        This method should be redefined if using something other than DiGMState.
        If only subclassing GraphMatcher, a redefinition is not necessary.
        */

        // core_1[n] contains the index of the node paired with n, which is m,
        //           provided n is in the mapping.
        // core_2[m] contains the index of the node paired with m, which is n,
        //           provided m is in the mapping.
        this->core_1 = {};
        this->core_2 = {};

        // See the paper for definitions of M_x and T_x^{y};

        // in_1[n]  is non-zero if n is in M_1 or in T_1^{in};
        // out_1[n] is non-zero if n is in M_1 or in T_1^{out};
        //
        // in_2[m]  is non-zero if m is in M_2 or in T_2^{in};
        // out_2[m] is non-zero if m is in M_2 or in T_2^{out};
        //
        // The value stored is the depth of the search tree when the node became
        // part of the corresponding set.
        this->in_1 = {};
        this->in_2 = {};
        this->out_1 = {};
        this->out_2 = {};

        this->state = DiGMState(*this);

        // Provide a convenient way to access the isomorphism mapping.
        this->mapping = this->core_1.copy();

    auto syntactic_feasibility(G1_node, G2_node) const -> void {
        /** Returns true if adding (G1_node, G2_node) is syntactically feasible.

        This function returns true if it is adding the candidate pair
        to the current partial isomorphism/monomorphism mapping is allowable.
        The addition is allowable if the inclusion of the candidate pair does
        not make it impossible for an isomorphism/monomorphism to be found.
        */

        // The VF2 algorithm was designed to work with graphs having, at most,
        // one edge connecting any two nodes.  This is not the case when
        // dealing with an MultiGraphs.
        //
        // Basically, when we test the look-ahead rules R_pred and R_succ, we
        // will make sure that the number of edges are checked.  We also add
        // a R_self check to verify that the number of selfloops is acceptable.

        // Users might be comparing DiGraph instances with MultiDiGraph
        // instances. So the generic DiGraphMatcher class must work with
        // MultiDiGraphs. Care must be taken since the value in the innermost
        // dictionary is a singlet for DiGraph instances.  For MultiDiGraphs,
        // the value in the innermost dictionary is a list.

        //##
        // Test at each step to get a return value as soon as possible.
        //##

        // Look ahead 0

        // R_self

        // The number of selfloops for G1_node must equal the number of
        // self-loops for G2_node. Without this check, we would fail on R_pred
        // at the next recursion level. This should prune the tree even further.
        if (this->test == "mono") {
            if (this->G1.number_of_edges(G1_node, G1_node) < this->G2.number_of_edges(
                G2_node, G2_node
            )) {
                return false;
        } else {
            if (this->G1.number_of_edges(G1_node, G1_node) != this->G2.number_of_edges(
                G2_node, G2_node
            )) {
                return false;

        // R_pred

        // For each predecessor n' of n in the partial mapping, the
        // corresponding node m' is a predecessor of m, and vice versa. Also,
        // the number of edges must be equal
        if (this->test != "mono") {
            for (auto predecessor : this->G1.pred[G1_node]) {
                if (this->core_1.contains(predecessor)) {
                    if (!(this->core_1[predecessor] in this->G2.pred[G2_node])) {
                        return false;
                    } else if this->G1.number_of_edges(
                        predecessor, G1_node
                    ) != this->G2.number_of_edges(this->core_1[predecessor], G2_node):
                        return false;

        for (auto predecessor : this->G2.pred[G2_node]) {
            if (this->core_2.contains(predecessor)) {
                if (!(this->core_2[predecessor] in this->G1.pred[G1_node])) {
                    return false;
                } else if (this->test == "mono") {
                    if (this->G1.number_of_edges(
                        this->core_2[predecessor], G1_node
                    ) < this->G2.number_of_edges(predecessor, G2_node)) {
                        return false;
                } else {
                    if (this->G1.number_of_edges(
                        this->core_2[predecessor], G1_node
                    ) != this->G2.number_of_edges(predecessor, G2_node)) {
                        return false;

        // R_succ

        // For each successor n' of n in the partial mapping, the corresponding
        // node m' is a successor of m, and vice versa. Also, the number of
        // edges must be equal.
        if (this->test != "mono") {
            for (auto successor : this->G1[G1_node]) {
                if (this->core_1.contains(successor)) {
                    if (!(this->core_1[successor] in this->G2[G2_node])) {
                        return false;
                    } else if this->G1.number_of_edges(
                        G1_node, successor
                    ) != this->G2.number_of_edges(G2_node, this->core_1[successor]):
                        return false;

        for (auto successor : this->G2[G2_node]) {
            if (this->core_2.contains(successor)) {
                if (!(this->core_2[successor] in this->G1[G1_node])) {
                    return false;
                } else if (this->test == "mono") {
                    if (this->G1.number_of_edges(
                        G1_node, this->core_2[successor];
                    ) < this->G2.number_of_edges(G2_node, successor)) {
                        return false;
                } else {
                    if (this->G1.number_of_edges(
                        G1_node, this->core_2[successor];
                    ) != this->G2.number_of_edges(G2_node, successor)) {
                        return false;

        if (this->test != "mono") {

            // Look ahead 1

            // R_termin
            // The number of predecessors of n that are in T_1^{in} is equal to the
            // number of predecessors of m that are in T_2^{in}.
            num1 = 0;
            for (auto predecessor : this->G1.pred[G1_node]) {
                if ((predecessor in this->in_1) and (!this->core_1.contains(predecessor))) {
                    num1 += 1;
            num2 = 0;
            for (auto predecessor : this->G2.pred[G2_node]) {
                if ((predecessor in this->in_2) and (!this->core_2.contains(predecessor))) {
                    num2 += 1;
            if (this->test == "graph") {
                if (!(num1 == num2)) {
                    return false;
            } else {  // this->test == 'subgraph'
                if (!(num1 >= num2)) {
                    return false;

            // The number of successors of n that are in T_1^{in} is equal to the
            // number of successors of m that are in T_2^{in}.
            num1 = 0;
            for (auto successor : this->G1[G1_node]) {
                if ((successor in this->in_1) and (!this->core_1.contains(successor))) {
                    num1 += 1;
            num2 = 0;
            for (auto successor : this->G2[G2_node]) {
                if ((successor in this->in_2) and (!this->core_2.contains(successor))) {
                    num2 += 1;
            if (this->test == "graph") {
                if (!(num1 == num2)) {
                    return false;
            } else {  // this->test == 'subgraph'
                if (!(num1 >= num2)) {
                    return false;

            // R_termout

            // The number of predecessors of n that are in T_1^{out} is equal to the
            // number of predecessors of m that are in T_2^{out}.
            num1 = 0;
            for (auto predecessor : this->G1.pred[G1_node]) {
                if ((predecessor in this->out_1) and (!this->core_1.contains(predecessor))) {
                    num1 += 1;
            num2 = 0;
            for (auto predecessor : this->G2.pred[G2_node]) {
                if ((predecessor in this->out_2) and (!this->core_2.contains(predecessor))) {
                    num2 += 1;
            if (this->test == "graph") {
                if (!(num1 == num2)) {
                    return false;
            } else {  // this->test == 'subgraph'
                if (!(num1 >= num2)) {
                    return false;

            // The number of successors of n that are in T_1^{out} is equal to the
            // number of successors of m that are in T_2^{out}.
            num1 = 0;
            for (auto successor : this->G1[G1_node]) {
                if ((successor in this->out_1) and (!this->core_1.contains(successor))) {
                    num1 += 1;
            num2 = 0;
            for (auto successor : this->G2[G2_node]) {
                if ((successor in this->out_2) and (!this->core_2.contains(successor))) {
                    num2 += 1;
            if (this->test == "graph") {
                if (!(num1 == num2)) {
                    return false;
            } else {  // this->test == 'subgraph'
                if (!(num1 >= num2)) {
                    return false;

            // Look ahead 2

            // R_new

            // The number of predecessors of n that are neither in the core_1 nor
            // T_1^{in} nor T_1^{out} is equal to the number of predecessors of m
            // that are neither in core_2 nor T_2^{in} nor T_2^{out}.
            num1 = 0;
            for (auto predecessor : this->G1.pred[G1_node]) {
                if (!this->in_1.contains((predecessor)) and (!this->out_1.contains(predecessor))) {
                    num1 += 1;
            num2 = 0;
            for (auto predecessor : this->G2.pred[G2_node]) {
                if (!this->in_2.contains((predecessor)) and (!this->out_2.contains(predecessor))) {
                    num2 += 1;
            if (this->test == "graph") {
                if (!(num1 == num2)) {
                    return false;
            } else {  // this->test == 'subgraph'
                if (!(num1 >= num2)) {
                    return false;

            // The number of successors of n that are neither in the core_1 nor
            // T_1^{in} nor T_1^{out} is equal to the number of successors of m
            // that are neither in core_2 nor T_2^{in} nor T_2^{out}.
            num1 = 0;
            for (auto successor : this->G1[G1_node]) {
                if (!this->in_1.contains((successor)) and (!this->out_1.contains(successor))) {
                    num1 += 1;
            num2 = 0;
            for (auto successor : this->G2[G2_node]) {
                if (!this->in_2.contains((successor)) and (!this->out_2.contains(successor))) {
                    num2 += 1;
            if (this->test == "graph") {
                if (!(num1 == num2)) {
                    return false;
            } else {  // this->test == 'subgraph'
                if (!(num1 >= num2)) {
                    return false;

        // Otherwise, this node pair is syntactically feasible!
        return true;
};

class GMState {
    /** Internal representation of state for the GraphMatcher class.

    This class is used internally by the GraphMatcher class.  It is used
    only to store state specific data. There will be at most G2.order() of
    these objects in memory at a time, due to the depth-first search
    strategy employed by the VF2 algorithm.
    */

    auto __init__(GM, G1_node=None, G2_node=None) const -> void {
        /** Initializes GMState object.

        Pass in the GraphMatcher to which this GMState belongs and the
        new node pair that will be added to the GraphMatcher's current
        isomorphism mapping.
        */
        this->GM = GM

        // Initialize the last stored node pair.
        this->G1_node = None
        this->G2_node = None
        this->depth = GM.core_1.size();

        if (G1_node is None or G2_node is None) {
            // Then we reset the class variables
            GM.core_1 = {};
            GM.core_2 = {};
            GM.inout_1 = {};
            GM.inout_2 = {};

        // Watch out! G1_node == 0 should evaluate to true.
        if (G1_node is not None and G2_node is not None) {
            // Add the node pair to the isomorphism mapping.
            GM.core_1[G1_node] = G2_node
            GM.core_2[G2_node] = G1_node

            // Store the node that was added last.
            this->G1_node = G1_node
            this->G2_node = G2_node

            // Now we must update the other two vectors.
            // We will add only if it is not in there already!
            this->depth = GM.core_1.size();

            // First we add the new nodes...
            if (!GM.inout_1.contains(G1_node)) {
                GM.inout_1[G1_node] = this->depth
            if (!GM.inout_2.contains(G2_node)) {
                GM.inout_2[G2_node] = this->depth

            // Now we add every other node...

            // Updates for T_1^{inout};
            new_nodes = set();
            for (auto node : GM.core_1) {
                new_nodes.update(
                    [neighbor for neighbor in GM.G1[node] if neighbor not in GM.core_1];
                );
            for (auto node : new_nodes) {
                if (!GM.inout_1.contains(node)) {
                    GM.inout_1[node] = this->depth

            // Updates for T_2^{inout};
            new_nodes = set();
            for (auto node : GM.core_2) {
                new_nodes.update(
                    [neighbor for neighbor in GM.G2[node] if neighbor not in GM.core_2];
                );
            for (auto node : new_nodes) {
                if (!GM.inout_2.contains(node)) {
                    GM.inout_2[node] = this->depth

    auto restore() const -> void {
        /** Deletes the GMState object and restores the class variables.*/
        // First we remove the node that was added from the core vectors.
        // Watch out! G1_node == 0 should evaluate to true.
        if (this->G1_node is not None and this->G2_node is not None) {
            del this->GM.core_1[this->G1_node];
            del this->GM.core_2[this->G2_node];

        // Now we revert the other two vectors.
        // Thus, we delete all entries which have this depth level.
        for (auto vector : (this->GM.inout_1, this->GM.inout_2)) {
            for (auto node : list(vector.keys())) {
                if (vector[node] == this->depth) {
                    del vector[node];
};

class DiGMState {
    /** Internal representation of state for the DiGraphMatcher class.

    This class is used internally by the DiGraphMatcher class.  It is used
    only to store state specific data. There will be at most G2.order() of
    these objects in memory at a time, due to the depth-first search
    strategy employed by the VF2 algorithm.

    */

    auto __init__(GM, G1_node=None, G2_node=None) const -> void {
        /** Initializes DiGMState object.

        Pass in the DiGraphMatcher to which this DiGMState belongs and the
        new node pair that will be added to the GraphMatcher's current
        isomorphism mapping.
        */
        this->GM = GM

        // Initialize the last stored node pair.
        this->G1_node = None
        this->G2_node = None
        this->depth = GM.core_1.size();

        if (G1_node is None or G2_node is None) {
            // Then we reset the class variables
            GM.core_1 = {};
            GM.core_2 = {};
            GM.in_1 = {};
            GM.in_2 = {};
            GM.out_1 = {};
            GM.out_2 = {};

        // Watch out! G1_node == 0 should evaluate to true.
        if (G1_node is not None and G2_node is not None) {
            // Add the node pair to the isomorphism mapping.
            GM.core_1[G1_node] = G2_node
            GM.core_2[G2_node] = G1_node

            // Store the node that was added last.
            this->G1_node = G1_node
            this->G2_node = G2_node

            // Now we must update the other four vectors.
            // We will add only if it is not in there already!
            this->depth = GM.core_1.size();

            // First we add the new nodes...
            for (auto vector : (GM.in_1, GM.out_1)) {
                if (!vector.contains(G1_node)) {
                    vector[G1_node] = this->depth
            for (auto vector : (GM.in_2, GM.out_2)) {
                if (!vector.contains(G2_node)) {
                    vector[G2_node] = this->depth

            // Now we add every other node...

            // Updates for T_1^{in};
            new_nodes = set();
            for (auto node : GM.core_1) {
                new_nodes.update(
                    [
                        predecessor
                        for predecessor in GM.G1.predecessors(node);
                        if predecessor not in GM.core_1
                    ];
                );
            for (auto node : new_nodes) {
                if (!GM.in_1.contains(node)) {
                    GM.in_1[node] = this->depth

            // Updates for T_2^{in};
            new_nodes = set();
            for (auto node : GM.core_2) {
                new_nodes.update(
                    [
                        predecessor
                        for predecessor in GM.G2.predecessors(node);
                        if predecessor not in GM.core_2
                    ];
                );
            for (auto node : new_nodes) {
                if (!GM.in_2.contains(node)) {
                    GM.in_2[node] = this->depth

            // Updates for T_1^{out};
            new_nodes = set();
            for (auto node : GM.core_1) {
                new_nodes.update(
                    [
                        successor
                        for successor in GM.G1.successors(node);
                        if successor not in GM.core_1
                    ];
                );
            for (auto node : new_nodes) {
                if (!GM.out_1.contains(node)) {
                    GM.out_1[node] = this->depth

            // Updates for T_2^{out};
            new_nodes = set();
            for (auto node : GM.core_2) {
                new_nodes.update(
                    [
                        successor
                        for successor in GM.G2.successors(node);
                        if successor not in GM.core_2
                    ];
                );
            for (auto node : new_nodes) {
                if (!GM.out_2.contains(node)) {
                    GM.out_2[node] = this->depth

    auto restore() const -> void {
        /** Deletes the DiGMState object and restores the class variables.*/

        // First we remove the node that was added from the core vectors.
        // Watch out! G1_node == 0 should evaluate to true.
        if (this->G1_node is not None and this->G2_node is not None) {
            del this->GM.core_1[this->G1_node];
            del this->GM.core_2[this->G2_node];

        // Now we revert the other four vectors.
        // Thus, we delete all entries which have this depth level.
        for (auto vector : (this->GM.in_1, this->GM.in_2, this->GM.out_1, this->GM.out_2)) {
            for (auto node : list(vector.keys())) {
                if (vector[node] == this->depth) {
                    del vector[node];
