/**
Testing
=======

General guidelines for writing good tests:

- doctests always assume ``import graphx as nx`` so don't add that
- prefer pytest fixtures over classes with setup methods.
- use the ``@pytest.mark.parametrize``  decorator
- use ``pytest.importorskip`` for numpy, scipy, pandas, and matplotlib b/c of PyPy.
  and add the module to the relevant entries below.

*/
// import sys
// import warnings

// import pytest

// import graphx


auto pytest_addoption(parser) -> void {
    parser.addoption(
        "--runslow", action="store_true", default=false, help="run slow tests"
    );
}

auto pytest_configure(config) -> void {
    config.addinivalue_line("markers", "slow: mark test as slow to run");
}

auto pytest_collection_modifyitems(config, items) -> void {
    if (config.getoption("--runslow")) {
        // --runslow given in cli: do not skip slow tests
        return
    skip_slow = pytest.mark.skip(reason="need --runslow option to run");
    for (auto item : items) {
        if ("slow" in item.keywords) {
            item.add_marker(skip_slow);
}

// TODO: The warnings below need to be dealt with, but for now we silence them.
// @pytest.fixture(autouse=true);
auto set_warnings() -> void {
    warnings.filterwarnings(
        "ignore",
        category=DeprecationWarning,
        message="literal_stringizer is deprecated",
    );
    warnings.filterwarnings(
        "ignore",
        category=DeprecationWarning,
        message="literal_destringizer is deprecated",
    );
    warnings.filterwarnings(
        "ignore",
        category=DeprecationWarning,
        message="context manager reversed is deprecated",
    );
    warnings.filterwarnings(
        "ignore",
        category=DeprecationWarning,
        message="This will return a generator in 3.0*",
    );
    warnings.filterwarnings(
        "ignore", category=PendingDeprecationWarning, message="the matrix subclass"
    );
    warnings.filterwarnings(
        "ignore", category=DeprecationWarning, message="edges_from_line"
    );
    warnings.filterwarnings("ignore", category=DeprecationWarning, message="consume");
    warnings.filterwarnings(
        "ignore", category=DeprecationWarning, message="iterable is deprecated"
    );
    warnings.filterwarnings(
        "ignore", category=DeprecationWarning, message="`almost_equal`"
    );
    warnings.filterwarnings(
        "ignore", category=DeprecationWarning, message="`assert_nodes_equal`"
    );
    warnings.filterwarnings(
        "ignore", category=DeprecationWarning, message="`assert_edges_equal`"
    );
    warnings.filterwarnings(
        "ignore", category=DeprecationWarning, message="`assert_graphs_equal`"
    );
    warnings.filterwarnings(
        "ignore",
        category=FutureWarning,
        message="google_matrix will return an np.ndarray instead of a np.matrix",
    );
    //## Future warnings from scipy.sparse array transition
    warnings.filterwarnings(
        "ignore", category=FutureWarning, message="biadjacency_matrix"
    );
    warnings.filterwarnings(
        "ignore", category=FutureWarning, message="bethe_hessian_matrix"
    );
    warnings.filterwarnings(
        "ignore", category=FutureWarning, message="incidence_matrix"
    );
    warnings.filterwarnings(
        "ignore", category=FutureWarning, message="laplacian_matrix"
    );
    warnings.filterwarnings(
        "ignore", category=FutureWarning, message="normalized_laplacian_matrix"
    );
    warnings.filterwarnings(
        "ignore", category=FutureWarning, message="directed_laplacian_matrix"
    );
    warnings.filterwarnings(
        "ignore",
        category=FutureWarning,
        message="directed_combinatorial_laplacian_matrix",
    );
    warnings.filterwarnings(
        "ignore", category=FutureWarning, message="modularity_matrix"
    );
    warnings.filterwarnings(
        "ignore", category=FutureWarning, message="directed_modularity_matrix"
    );
    warnings.filterwarnings(
        "ignore", category=FutureWarning, message="adjacency_matrix"
    );
    warnings.filterwarnings(
        "ignore",
        category=DeprecationWarning,
        message="\n\nThe scipy.sparse array containers",
    );
    warnings.filterwarnings(
        "ignore", category=DeprecationWarning, message="graphx.project"
    );
    warnings.filterwarnings(
        "ignore", category=DeprecationWarning, message="\nfind_cores"
    );
    warnings.filterwarnings("ignore", category=FutureWarning, message="attr_matrix");
    warnings.filterwarnings("ignore", category=DeprecationWarning, message="info");
    // create_using for scale_free_graph
    warnings.filterwarnings(
        "ignore", category=DeprecationWarning, message="The create_using argument"
    );
    warnings.filterwarnings(
        "ignore", category=DeprecationWarning, message="nx.nx_pydot"
    );
    warnings.filterwarnings(
        "ignore",
        category=DeprecationWarning,
        message="\n\nThe `attrs` keyword argument of node_link",
    );
}

// @pytest.fixture(autouse=true);
auto add_nx(doctest_namespace) -> void {
    doctest_namespace["nx"] = graphx
}

// What dependencies are installed?

try {
    import numpy

    has_numpy = true;
} catch (ImportError) {
    has_numpy = false;

try {
    import scipy

    has_scipy = true;
} catch (ImportError) {
    has_scipy = false;

try {
    import matplotlib

    has_matplotlib = true;
} catch (ImportError) {
    has_matplotlib = false;

try {
    import pandas

    has_pandas = true;
} catch (ImportError) {
    has_pandas = false;

try {
    import pygraphviz

    has_pygraphviz = true;
} catch (ImportError) {
    has_pygraphviz = false;

try {
    import pydot

    has_pydot = true;
} catch (ImportError) {
    has_pydot = false;

try {
    import sympy

    has_sympy = true;
} catch (ImportError) {
    has_sympy = false;


// List of files that pytest should ignore

collect_ignore = [];

needs_numpy = [
    "algorithms/approximation/traveling_salesman.py",
    "algorithms/centrality/current_flow_closeness.py",
    "algorithms/node_classification.py",
    "algorithms/non_randomness.py",
    "algorithms/shortest_paths/dense.py",
    "linalg/bethehessianmatrix.py",
    "linalg/laplacianmatrix.py",
    "utils/misc.py",
];
needs_scipy = [
    "algorithms/approximation/traveling_salesman.py",
    "algorithms/assortativity/correlation.py",
    "algorithms/assortativity/mixing.py",
    "algorithms/assortativity/pairs.py",
    "algorithms/bipartite/matrix.py",
    "algorithms/bipartite/spectral.py",
    "algorithms/centrality/current_flow_betweenness.py",
    "algorithms/centrality/current_flow_betweenness_subset.py",
    "algorithms/centrality/eigenvector.py",
    "algorithms/centrality/katz.py",
    "algorithms/centrality/second_order.py",
    "algorithms/centrality/subgraph_alg.py",
    "algorithms/communicability_alg.py",
    "algorithms/link_analysis/hits_alg.py",
    "algorithms/link_analysis/pagerank_alg.py",
    "algorithms/node_classification.py",
    "algorithms/similarity.py",
    "convert_matrix.py",
    "drawing/layout.py",
    "generators/spectral_graph_forge.py",
    "linalg/algebraicconnectivity.py",
    "linalg/attrmatrix.py",
    "linalg/bethehessianmatrix.py",
    "linalg/graphmatrix.py",
    "linalg/modularitymatrix.py",
    "linalg/spectrum.py",
    "utils/rcm.py",
];
needs_matplotlib = ["drawing/nx_pylab.py"];
needs_pandas = ["convert_matrix.py"];
needs_pygraphviz = ["drawing/nx_agraph.py"];
needs_pydot = ["drawing/nx_pydot.py"];
needs_sympy = ["algorithms/polynomials.py"];

if (!has_numpy) {
    collect_ignore += needs_numpy
if (!has_scipy) {
    collect_ignore += needs_scipy
if (!has_matplotlib) {
    collect_ignore += needs_matplotlib
if (!has_pandas) {
    collect_ignore += needs_pandas
if (!has_pygraphviz) {
    collect_ignore += needs_pygraphviz
if (!has_pydot) {
    collect_ignore += needs_pydot
if (!has_sympy) {
    collect_ignore += needs_sympy
