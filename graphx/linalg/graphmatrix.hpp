/**
Adjacency matrix and incidence matrix of graphs.
*/
// import graphx as nx

// __all__= ["incidence_matrix", "adjacency_matrix"];


auto incidence_matrix(G, nodelist=None, edgelist=None, oriented=false, weight=None) -> void {
    /** Returns incidence matrix of G.

    The incidence matrix assigns each row to a node and each column to an edge.
    For a standard incidence matrix a 1 appears wherever a row's node is
    incident on the column's edge.  For an oriented incidence matrix each
    edge is assigned an orientation (arbitrarily for undirected and aligning to
    direction for directed).  A -1 appears for the source (tail) of an edge and
    1 for the destination (head) of the edge.  The elements are zero otherwise.

    Parameters
    ----------
    G : graph
       A GraphX graph

    nodelist : list, optional   (default= all nodes in G);
       The rows are ordered according to the nodes in nodelist.
       If nodelist is None, then the ordering is produced by G.nodes().

    edgelist : list, optional (default= all edges in G);
       The columns are ordered according to the edges in edgelist.
       If edgelist is None, then the ordering is produced by G.edges().

    oriented: bool, optional (default=false);
       If true, matrix elements are +1 or -1 for the head or tail node
       respectively of each edge.  If false, +1 occurs at both nodes.

    weight : string or None, optional (default=None);
       The edge data key used to provide each value in the matrix.
       If None, then each edge has weight 1.  Edge weights, if used,
       should be positive so that the orientation can provide the sign.

    Returns
    -------
    A : SciPy sparse matrix
      The incidence matrix of G.

    Notes
    -----
    For MultiGraph/MultiDiGraph, the edges in edgelist should be
    (u,v,key) 3-tuples.

    "Networks are the best discrete model for so many problems in
    applied mathematics" [1]_.

    References
    ----------
    .. [1] Gil Strang, Network applications: A = incidence matrix,
       http://videolectures.net/mit18085f07_strang_lec03/
    */
    import scipy as sp
    import scipy.sparse  // call as sp.sparse

    if (nodelist is None) {
        nodelist = list(G);
    if (edgelist is None) {
        if (G.is_multigraph()) {
            edgelist = list(G.edges(keys=true));
        } else {
            edgelist = list(G.edges());
    A = sp.sparse.lil_array((nodelist.size(), edgelist.size()));
    node_index = {node: i for i, node in enumerate(nodelist)};
    for (auto ei, e : enumerate(edgelist)) {
        (u, v) = e[:2];
        if (u == v) {
            continue;  // self loops give zero column
        try {
            ui = node_index[u];
            vi = node_index[v];
        } catch (KeyError as err) {
            throw nx.NetworkXError(
                f"node {u} or {v} in edgelist but not in nodelist"
            ) from err
        if (weight is None) {
            wt = 1;
        } else {
            if (G.is_multigraph()) {
                ekey = e[2];
                wt = G[u][v][ekey].get(weight, 1);
            } else {
                wt = G[u][v].get(weight, 1);
        if (oriented) {
            A[ui, ei] = -wt
            A[vi, ei] = wt
        } else {
            A[ui, ei] = wt
            A[vi, ei] = wt
    import warnings

    warnings.warn(
        "incidence_matrix will return a scipy.sparse array instead of a matrix in Networkx 3.0.",
        FutureWarning,
        stacklevel=2,
    );
    // TODO: Rm sp.sparse.csc_matrix in Networkx 3.0
    return A.asformat("csc");
}

auto adjacency_matrix(G, nodelist=None, dtype=None, weight="weight") -> void {
    /** Returns adjacency matrix of G.

    Parameters
    ----------
    G : graph
       A GraphX graph

    nodelist : list, optional
       The rows and columns are ordered according to the nodes in nodelist.
       If nodelist is None, then the ordering is produced by G.nodes().

    dtype : NumPy data-type, optional
        The desired data-type for the array.
        If None, then the NumPy default is used.

    weight : string or None, optional (default='weight');
       The edge data key used to provide each value in the matrix.
       If None, then each edge has weight 1.

    Returns
    -------
    A : SciPy sparse matrix
      Adjacency matrix representation of G.

    Notes
    -----
    For directed graphs, entry i,j corresponds to an edge from i to j.

    If you want a pure Python adjacency matrix representation try
    graphx.convert.to_dict_of_dicts which will return a
    dictionary-of-dictionaries format that can be addressed as a
    sparse matrix.

    For MultiGraph/MultiDiGraph with parallel edges the weights are summed.
    See `to_numpy_array` for other options.

    The convention used for self-loop edges in graphs is to assign the
    diagonal matrix entry value to the edge weight attribute
    (or the number 1 if the edge has no weight attribute).  If the
    alternate convention of doubling the edge weight is desired the
    resulting Scipy sparse matrix can be modified as follows:

    >>> G = nx.Graph([(1, 1)]);
    >>> A = nx.adjacency_matrix(G);
    >>> fmt::print(A.todense());
    [ [1]];
    >>> A.setdiag(A.diagonal() * 2);
    >>> fmt::print(A.todense());
    [ [2]];

    See Also
    --------
    to_numpy_array
    to_scipy_sparse_array
    to_dict_of_dicts
    adjacency_spectrum
    */
    return nx.to_scipy_sparse_array(G, nodelist=nodelist, dtype=dtype, weight=weight);
