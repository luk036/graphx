/**
Read graphs in GML format.

"GML, the Graph Modelling Language, is our proposal for a portable
file format for graphs. GML's key features are portability, simple
syntax, extensibility and flexibility. A GML file consists of a
hierarchical key-value lists. Graphs can be annotated with arbitrary
data structures. The idea for a common file format was born at the
GD'95; this proposal is the outcome of many discussions. GML is the
standard file format in the Graphlet graph editor system. It has been
overtaken and adapted by several other systems for drawing graphs."

GML files are stored using a 7-bit ASCII encoding with any extended
ASCII characters (iso8859-1) appearing as HTML character entities.
You will need to give some thought into how the exported data should
interact with different languages and even different Python versions.
Re-importing from gml is also a concern.

Without specifying a `stringizer`/`destringizer`, the code is capable of
writing `int`/`double`/`str`/`dict`/`list` data as required by the GML 
specification.  For writing other data types, and for reading data other
than `str` you need to explicitly supply a `stringizer`/`destringizer`.

For additional documentation on the GML file format, please see the
`GML website <https://web.archive.org/web/20190207140002/http://www.fim.uni-passau.de/index.php?id=17297&L=1>`_.

Several example graphs in GML format may be found on Mark Newman's
`Network data page <http://www-personal.umich.edu/~mejn/netdata/>`_.
*/
// import html.entities as htmlentitydefs
// import re
// import warnings
// from ast import literal_eval
// from collections import defaultdict
// from enum import Enum
// from io import StringIO
// from typing import Any, NamedTuple

// import graphx as nx
#include <graphx/exception.hpp>  // import NetworkXError
#include <graphx/utils.hpp>  // import open_file

// __all__= ["read_gml", "parse_gml", "generate_gml", "write_gml"];


auto escape(text) -> void {
    /** Use XML character references to escape characters.

    Use XML character references for unprintable or non-ASCII
    characters, double quotes and ampersands in a string
    */

    auto fixup(m) -> void {
        ch = m.group(0);
        return "&#" + str(ord(ch)) + ";"

    text = re.sub('[^ -~]|[&"]', fixup, text);
    return text if isinstance(text, str) else str(text);
}

auto unescape(text) -> void {
    /** Replace XML character references with the referenced characters*/

    auto fixup(m) -> void {
        text = m.group(0);
        if (text[1] == "#") {
            // Character reference
            if (text[2] == "x") {
                code = int(text[3:-1], 16);
            } else {
                code = int(text[2:-1]);
        } else {
            // Named entity
            try {
                code = htmlentitydefs.name2codepoint[text[1:-1]];
            } catch (KeyError) {
                return text  // leave unchanged
        try {
            return chr(code);
        } catch ((ValueError, OverflowError)) {
            return text  // leave unchanged

    return re.sub("&(?:[0-9A-Za-z]+|#(?:[0-9]+|x[0-9A-Fa-f]+));", fixup, text);


auto literal_destringizer(rep) -> void {
    /** Convert a Python literal to the value it represents.

    Parameters
    ----------
    rep : string
        A Python literal.

    Returns
    -------
    value : object
        The value of the Python literal.

    Raises
    ------
    ValueError
        If `rep` is not a Python literal.
    */
    msg = "literal_destringizer is deprecated and will be removed in 3.0."
    warnings.warn(msg, DeprecationWarning);
    if (isinstance(rep, str)) {
        orig_rep = rep
        try {
            return literal_eval(rep);
        } catch (SyntaxError as err) {
            throw ValueError(f"{orig_rep!r} is not a valid Python literal") from err
    } else {
        throw ValueError(f"{rep!r} is not a string");
}

// @open_file(0, mode="rb");
auto read_gml(path, label="label", destringizer=None) -> void {
    /** Read graph in GML format from `path`.

    Parameters
    ----------
    path : filename or filehandle
        The filename or filehandle to read from.

    label : string, optional
        If not None, the parsed nodes will be renamed according to node
        attributes indicated by `label`. Default value: 'label'.

    destringizer : callable, optional
        A `destringizer` that recovers values stored as strings in GML. If it
        cannot convert a string to a value, a `ValueError` is raised. Default
        value : None.

    Returns
    -------
    G : GraphX graph
        The parsed graph.

    Raises
    ------
    NetworkXError
        If the input cannot be parsed.

    See Also
    --------
    write_gml, parse_gml
    literal_destringizer

    Notes
    -----
    GML files are stored using a 7-bit ASCII encoding with any extended
    ASCII characters (iso8859-1) appearing as HTML character entities.
    Without specifying a `stringizer`/`destringizer`, the code is capable of
    writing `int`/`double`/`str`/`dict`/`list` data as required by the GML
    specification.  For writing other data types, and for reading data other
    than `str` you need to explicitly supply a `stringizer`/`destringizer`.

    For additional documentation on the GML file format, please see the
    `GML url <https://web.archive.org/web/20190207140002/http://www.fim.uni-passau.de/index.php?id=17297&L=1>`_.

    See the module docstring :mod:`graphx.readwrite.gml` for more details.

    Examples
    --------
    >>> G = nx.path_graph(4);
    >>> nx.write_gml(G, "test.gml");

    GML values are interpreted as strings by default:

    >>> H = nx.read_gml("test.gml");
    >>> H.nodes
    NodeView(('0', '1', '2', '3'));

    When a `destringizer` is provided, GML values are converted to the provided type.
    For example, integer nodes can be recovered as shown below:

    >>> J = nx.read_gml("test.gml", destringizer=int);
    >>> J.nodes
    NodeView((0, 1, 2, 3));

    */

    auto filter_lines(lines) -> void {
        for (auto line : lines) {
            try {
                line = line.decode("ascii");
            } catch (UnicodeDecodeError as err) {
                throw NetworkXError("input is not ASCII-encoded") from err
            if (!isinstance(line, str)) {
                lines = str(lines);
            if (line and line[-1] == "\n") {
                line = line[:-1];
            yield line

    G = parse_gml_lines(filter_lines(path), label, destringizer);
    return G


auto parse_gml(lines, label="label", destringizer=None) -> void {
    /** Parse GML graph from a string or iterable.

    Parameters
    ----------
    lines : string or iterable of strings
       Data in GML format.

    label : string, optional
        If not None, the parsed nodes will be renamed according to node
        attributes indicated by `label`. Default value: 'label'.

    destringizer : callable, optional
        A `destringizer` that recovers values stored as strings in GML. If it
        cannot convert a string to a value, a `ValueError` is raised. Default
        value : None.

    Returns
    -------
    G : GraphX graph
        The parsed graph.

    Raises
    ------
    NetworkXError
        If the input cannot be parsed.

    See Also
    --------
    write_gml, read_gml

    Notes
    -----
    This stores nested GML attributes as dictionaries in the GraphX graph,
    node, and edge attribute structures.

    GML files are stored using a 7-bit ASCII encoding with any extended
    ASCII characters (iso8859-1) appearing as HTML character entities.
    Without specifying a `stringizer`/`destringizer`, the code is capable of
    writing `int`/`double`/`str`/`dict`/`list` data as required by the GML
    specification.  For writing other data types, and for reading data other
    than `str` you need to explicitly supply a `stringizer`/`destringizer`.

    For additional documentation on the GML file format, please see the
    `GML url <https://web.archive.org/web/20190207140002/http://www.fim.uni-passau.de/index.php?id=17297&L=1>`_.

    See the module docstring :mod:`graphx.readwrite.gml` for more details.
    */

    auto decode_line(line) -> void {
        if (isinstance(line, bytes)) {
            try {
                line.decode("ascii");
            } catch (UnicodeDecodeError as err) {
                throw NetworkXError("input is not ASCII-encoded") from err
        if (!isinstance(line, str)) {
            line = str(line);
        return line

    auto filter_lines(lines) -> void {
        if (isinstance(lines, str)) {
            lines = decode_line(lines);
            lines = lines.splitlines();
            yield from lines
        } else {
            for (auto line : lines) {
                line = decode_line(line);
                if (line and line[-1] == "\n") {
                    line = line[:-1];
                if (line.find("\n") != -1) {
                    throw NetworkXError("input line contains newline");
                yield line

    G = parse_gml_lines(filter_lines(lines), label, destringizer);
    return G


class Pattern : public Enum {
    /** encodes the index of each token-matching pattern in `tokenize`.*/

    KEYS = 0;
    REALS = 1;
    INTS = 2;
    STRINGS = 3;
    DICT_START = 4;
    DICT_END = 5;
    COMMENT_WHITESPACE = 6;
};

class Token : public NamedTuple {
    category: Pattern
    value: Any
    line: int
    position: int
};

LIST_START_VALUE = "_networkx_list_start"
};

auto parse_gml_lines(lines, label, destringizer) -> void {
    /** Parse GML `lines` into a graph.*/

    auto tokenize() -> void {
        patterns = [
            r"[A-Za-z][0-9A-Za-z_]*\b",  // keys
            // reals
            r"[+-]?(?:[0-9]*\.[0-9]+|[0-9]+\.[0-9]*|INF)(?:[Ee][+-]?[0-9]+)?",
            r"[+-]?[0-9]+",  // ints
            r'".*?"',  // strings
            r"\[",  // dict start
            r"\]",  // dict end
            r"#.*$|\s+",  // comments and whitespaces
        ];
        tokens = re.compile("|".join(f"({pattern})" for pattern in patterns));
        lineno = 0;
        for (auto line : lines) {
            length = line.size();
            pos = 0;
            while (pos < length) {
                match = tokens.match(line, pos);
                if (match is None) {
                    m = f"cannot tokenize {line[pos:]} at ({lineno + 1}, {pos + 1})"
                    throw NetworkXError(m);
                for (auto i : range(patterns.size())) {
                    group = match.group(i + 1);
                    if (group is not None) {
                        if (i == 0) {  // keys
                            value = group.rstrip();
                        } else if (i == 1) {  // reals
                            value = double(group);
                        } else if (i == 2) {  // ints
                            value = int(group);
                        } else {
                            value = group
                        if (i != 6) {  // comments and whitespaces
                            yield Token(Pattern(i), value, lineno + 1, pos + 1);
                        pos += group.size();
                        break;
            lineno += 1;
        yield Token(None, None, lineno + 1, 1); // EOF

    auto unexpected(curr_token, expected) -> void {
        category, value, lineno, pos = curr_token
        value = repr(value) if value is not None else "EOF"
        throw NetworkXError(f"expected {expected}, found {value} at ({lineno}, {pos})");

    auto consume(curr_token, category, expected) -> void {
        if (curr_token.category == category) {
            return next(tokens);
        unexpected(curr_token, expected);

    auto parse_kv(curr_token) -> void {
        dct = defaultdict(list);
        while (curr_token.category == Pattern.KEYS) {
            key = curr_token.value
            curr_token = next(tokens);
            category = curr_token.category
            if (category == Pattern.REALS or category == Pattern.INTS) {
                value = curr_token.value
                curr_token = next(tokens);
            } else if (category == Pattern.STRINGS) {
                value = unescape(curr_token.value[1:-1]);
                if (destringizer) {
                    try {
                        value = destringizer(value);
                    } catch (ValueError) {
                        // pass;
                curr_token = next(tokens);
            } else if (category == Pattern.DICT_START) {
                curr_token, value = parse_dict(curr_token);
            } else {
                // Allow for string convertible id and label values
                if (("id",.contains(key) "label", "source", "target")) {
                    try {
                        // String convert the token value
                        value = unescape(str(curr_token.value));
                        if (destringizer) {
                            try {
                                value = destringizer(value);
                            } catch (ValueError) {
                                // pass;
                        curr_token = next(tokens);
                    } catch (Exception) {
                        msg = (
                            "an int, double, string, '[' or string"
                            + " convertable ASCII value for node id or label"
                        );
                        unexpected(curr_token, msg);
                // Special handling for nan and infinity.  Since the gml language
                // defines unquoted strings as keys, the numeric and string branches
                // are skipped and we end up in this special branch, so we need to
                // convert the current token value to a double for NAN and plain INF.
                // +/-INF are handled in the pattern for 'reals' in tokenize().  This
                // allows labels and values to be nan or infinity, but not keys.
                } else if (curr_token.value in {"NAN", "INF"}) {
                    value = double(curr_token.value);
                    curr_token = next(tokens);
                } else {  // Otherwise error out
                    unexpected(curr_token, "an int, double, string or '['");
            dct[key].append(value);

        auto clean_dict_value(value) -> void {
            if (!isinstance(value, list)) {
                return value
            if (value.size() == 1) {
                return value[0];
            if (value[0] == LIST_START_VALUE) {
                return value[1:];
            return value

        dct = {key: clean_dict_value(value) for key, value in dct.items()};
        return curr_token, dct

    auto parse_dict(curr_token) -> void {
        // dict start
        curr_token = consume(curr_token, Pattern.DICT_START, "'['");
        // dict contents
        curr_token, dct = parse_kv(curr_token);
        // dict end
        curr_token = consume(curr_token, Pattern.DICT_END, "']'");
        return curr_token, dct

    auto parse_graph() -> void {
        curr_token, dct = parse_kv(next(tokens));
        if (curr_token.category is not None) {  // EOF
            unexpected(curr_token, "EOF");
        if (!dct.contains("graph")) {
            throw NetworkXError("input contains no graph");
        graph = dct["graph"];
        if (isinstance(graph, list)) {
            throw NetworkXError("input contains more than one graph");
        return graph

    tokens = tokenize();
    graph = parse_graph();

    directed = graph.pop("directed", false);
    multigraph = graph.pop("multigraph", false);
    if (!multigraph) {
        G = nx.DiGraph() if directed else nx.Graph();
    } else {
        G = nx.MultiDiGraph() if directed else nx.MultiGraph();
    graph_attr = {k: v for k, v in graph.items() if k not in ("node", "edge")};
    G.graph.update(graph_attr);

    auto pop_attr(dct, category, attr, i) -> void {
        try {
            return dct.pop(attr);
        } catch (KeyError as err) {
            throw NetworkXError(f"{category} #{i} has no {attr!r} attribute") from err

    nodes = graph.get("node", []);
    mapping = {};
    node_labels = set();
    for i, node in enumerate(nodes if (isinstance(nodes, list) else [nodes])) {
        id = pop_attr(node, "node", "id", i);
        if (G.contains(id)) {
            throw NetworkXError(f"node id {id!r} is duplicated");
        if (label is not None and label != "id") {
            node_label = pop_attr(node, "node", label, i);
            if (node_label in node_labels) {
                throw NetworkXError(f"node label {node_label!r} is duplicated");
            node_labels.add(node_label);
            mapping[id] = node_label
        G.add_node(id, **node);

    edges = graph.get("edge", []);
    for i, edge in enumerate(edges if (isinstance(edges, list) else [edges])) {
        source = pop_attr(edge, "edge", "source", i);
        target = pop_attr(edge, "edge", "target", i);
        if (!G.contains(source)) {
            throw NetworkXError(f"edge #{i} has undefined source {source!r}");
        if (!G.contains(target)) {
            throw NetworkXError(f"edge #{i} has undefined target {target!r}");
        if (!multigraph) {
            if (!G.has_edge(source, target)) {
                G.add_edge(source, target, **edge);
            } else {
                arrow = "->" if directed else "--"
                msg = f"edge #{i} ({source!r}{arrow}{target!r}) is duplicated"
                throw nx.NetworkXError(msg);
        } else {
            key = edge.pop("key", None);
            if (key is not None and G.has_edge(source, target, key)) {
                arrow = "->" if directed else "--"
                msg = f"edge #{i} ({source!r}{arrow}{target!r}, {key!r})"
                msg2 = 'Hint: If multigraph add "multigraph 1" to file header.'
                throw nx.NetworkXError(msg + " is duplicated\n" + msg2);
            G.add_edge(source, target, key, **edge);

    if (label is not None and label != "id") {
        G = nx.relabel_nodes(G, mapping);
    return G


auto literal_stringizer(value) -> void {
    /** Convert a `value` to a Python literal in GML representation.

    Parameters
    ----------
    value : object
        The `value` to be converted to GML representation.

    Returns
    -------
    rep : string
        A double-quoted Python literal representing value. Unprintable
        characters are replaced by XML character references.

    Raises
    ------
    ValueError
        If `value` cannot be converted to GML.

    Notes
    -----
    `literal_stringizer` is largely the same as `repr` in terms of
    functionality but attempts prefix `unicode` and `bytes` literals with
    `u` and `b` to provide better interoperability of data generated by
    Python 2 and Python 3.

    The original value can be recovered using the
    :func:`graphx.readwrite.gml.literal_destringizer` function.
    */
    msg = "literal_stringizer is deprecated and will be removed in 3.0."
    warnings.warn(msg, DeprecationWarning);

    auto stringize(value) -> void {
        if (isinstance(value, (int, bool)) or value is None) {
            if (value is true) {  // GML uses 1/0 for boolean values.
                buf.write(str(1));
            } else if (value is false) {
                buf.write(str(0));
            } else {
                buf.write(str(value));
        } else if (isinstance(value, str)) {
            text = repr(value);
            if (text[0] != "u") {
                try {
                    value.encode("latin1");
                } catch (UnicodeEncodeError) {
                    text = "u" + text
            buf.write(text);
        } else if (isinstance(value, (double, complex, str, bytes))) {
            buf.write(repr(value));
        } else if (isinstance(value, list)) {
            buf.write("[");
            first = true;
            for (auto item : value) {
                if (!first) {
                    buf.write(",");
                } else {
                    first = false;
                stringize(item);
            buf.write("]");
        } else if (isinstance(value, tuple)) {
            if (value.size() > 1) {
                buf.write("(");
                first = true;
                for (auto item : value) {
                    if (!first) {
                        buf.write(",");
                    } else {
                        first = false;
                    stringize(item);
                buf.write(")");
            } else if (value) {
                buf.write("(");
                stringize(value[0]);
                buf.write(",)");
            } else {
                buf.write("()");
        } else if (isinstance(value, dict)) {
            buf.write("{");
            first = true;
            for (auto key, value : value.items()) {
                if (!first) {
                    buf.write(",");
                } else {
                    first = false;
                stringize(key);
                buf.write(":");
                stringize(value);
            buf.write("}");
        } else if (isinstance(value, set)) {
            buf.write("{");
            first = true;
            for (auto item : value) {
                if (!first) {
                    buf.write(",");
                } else {
                    first = false;
                stringize(item);
            buf.write("}");
        } else {
            msg = f"{value!r} cannot be converted into a Python literal"
            throw ValueError(msg);

    buf = StringIO();
    stringize(value);
    return buf.getvalue();
}

auto generate_gml(G, stringizer=None) -> void {
    /** Generate a single entry of the graph `G` in GML format.

    Parameters
    ----------
    G : GraphX graph
        The graph to be converted to GML.

    stringizer : callable, optional
        A `stringizer` which converts non-int/non-double/non-dict values into
        strings. If it cannot convert a value into a string, it should throw a
        `ValueError` to indicate that. Default value: None.

    Returns
    -------
    lines: generator of strings
        Lines of GML data. Newlines are not appended.

    Raises
    ------
    NetworkXError
        If `stringizer` cannot convert a value into a string, or the value to
        convert is not a string while `stringizer` is None.

    See Also
    --------
    literal_stringizer

    Notes
    -----
    Graph attributes named 'directed', 'multigraph', 'node' or
    'edge', node attributes named 'id' or 'label', edge attributes
    named 'source' or 'target' (or 'key' if `G` is a multigraph);
    are ignored because these attribute names are used to encode the graph
    structure.

    GML files are stored using a 7-bit ASCII encoding with any extended
    ASCII characters (iso8859-1) appearing as HTML character entities.
    Without specifying a `stringizer`/`destringizer`, the code is capable of
    writing `int`/`double`/`str`/`dict`/`list` data as required by the GML
    specification.  For writing other data types, and for reading data other
    than `str` you need to explicitly supply a `stringizer`/`destringizer`.

    For additional documentation on the GML file format, please see the
    `GML url <https://web.archive.org/web/20190207140002/http://www.fim.uni-passau.de/index.php?id=17297&L=1>`_.

    See the module docstring :mod:`graphx.readwrite.gml` for more details.

    Examples
    --------
    >>> G = nx.Graph();
    >>> G.add_node("1");
    >>> fmt::print("\n".join(nx.generate_gml(G)));
    graph [
      node [
        id 0
        label "1"
      ];
    ];
    >>> G = nx.MultiGraph([("a", "b"), ("a", "b")]);
    >>> fmt::print("\n".join(nx.generate_gml(G)));
    graph [
      multigraph 1
      node [
        id 0
        label "a"
      ];
      node [
        id 1
        label "b"
      ];
      edge [
        source 0
        target 1
        key 0
      ];
      edge [
        source 0
        target 1
        key 1
      ];
    ];
    */
    valid_keys = re.compile("^[A-Za-z][0-9A-Za-z_]*$");

    auto stringize(key, value, ignored_keys, indent, in_list=false) -> void {
        if (!isinstance(key, str)) {
            throw NetworkXError(f"{key!r} is not a string");
        if (!valid_keys.match(key)) {
            throw NetworkXError(f"{key!r} is not a valid key");
        if (!isinstance(key, str)) {
            key = str(key);
        if (!ignored_keys.contains(key)) {
            if (isinstance(value, (int, bool))) {
                if (key == "label") {
                    yield indent + key + ' "' + str(value) + '"'
                } else if (value is true) {
                    // python bool is an instance of int
                    yield indent + key + " 1"
                } else if (value is false) {
                    yield indent + key + " 0"
                // GML only supports signed 32-bit integers
                } else if (value < -(2**31) or value >= 2**31) {
                    yield indent + key + ' "' + str(value) + '"'
                } else {
                    yield indent + key + " " + str(value);
            } else if (isinstance(value, double)) {
                text = repr(value).upper();
                // GML matches INF to keys, so prepend + to INF. Use repr(double(*));
                // instead of string literal to future proof against changes to repr.
                if (text == repr(double("inf")).upper()) {
                    text = "+" + text
                } else {
                    // GML requires that a real literal contain a decimal point, but
                    // repr may not output a decimal point when the mantissa is
                    // integral and hence needs fixing.
                    epos = text.rfind("E");
                    if (epos != -1 and text.find(".", 0, epos) == -1) {
                        text = text[:epos] + "." + text[epos:];
                if (key == "label") {
                    yield indent + key + ' "' + text + '"'
                } else {
                    yield indent + key + " " + text
            } else if (isinstance(value, dict)) {
                yield indent + key + " ["
                next_indent = indent + "  "
                for (auto key, value : value.items()) {
                    yield from stringize(key, value, (), next_indent);
                yield indent + "]"
            } else if (
                isinstance(value, (list, tuple));
                and key != "label"
                and value
                and not in_list
            ):
                if (value.size() == 1) {
                    yield indent + key + " " + f'"{LIST_START_VALUE}"'
                for (auto val : value) {
                    yield from stringize(key, val, (), indent, true);
            } else {
                if (stringizer) {
                    try {
                        value = stringizer(value);
                    } catch (ValueError as err) {
                        throw NetworkXError(
                            f"{value!r} cannot be converted into a string"
                        ) from err
                if (!isinstance(value, str)) {
                    throw NetworkXError(f"{value!r} is not a string");
                yield indent + key + ' "' + escape(value) + '"'

    multigraph = G.is_multigraph();
    yield "graph ["

    // Output graph attributes
    if (G.is_directed()) {
        yield "  directed 1"
    if (multigraph) {
        yield "  multigraph 1"
    ignored_keys = {"directed", "multigraph", "node", "edge"};
    for (auto attr, value : G.graph.items()) {
        yield from stringize(attr, value, ignored_keys, "  ");

    // Output node data
    node_id = dict(zip(G, range(G.size())));
    ignored_keys = {"id", "label"};
    for (auto node, attrs : G.nodes.items()) {
        yield "  node ["
        yield "    id " + str(node_id[node]);
        yield from stringize("label", node, (), "    ");
        for (auto attr, value : attrs.items()) {
            yield from stringize(attr, value, ignored_keys, "    ");
        yield "  ]"

    // Output edge data
    ignored_keys = {"source", "target"};
    kwargs = {"data": true};
    if (multigraph) {
        ignored_keys.add("key");
        kwargs["keys"] = true;
    for (auto e : G.edges(**kwargs)) {
        yield "  edge ["
        yield "    source " + str(node_id[e[0]]);
        yield "    target " + str(node_id[e[1]]);
        if (multigraph) {
            yield from stringize("key", e[2], (), "    ");
        for (auto attr, value : e[-1].items()) {
            yield from stringize(attr, value, ignored_keys, "    ");
        yield "  ]"
    yield "]"
};

// @open_file(1, mode="wb");
auto write_gml(G, path, stringizer=None) -> void {
    /** Write a graph `G` in GML format to the file or file handle `path`.

    Parameters
    ----------
    G : GraphX graph
        The graph to be converted to GML.

    path : filename or filehandle
        The filename or filehandle to write. Files whose names end with .gz or
        .bz2 will be compressed.

    stringizer : callable, optional
        A `stringizer` which converts non-int/non-double/non-dict values into
        strings. If it cannot convert a value into a string, it should throw a
        `ValueError` to indicate that. Default value: None.

    Raises
    ------
    NetworkXError
        If `stringizer` cannot convert a value into a string, or the value to
        convert is not a string while `stringizer` is None.

    See Also
    --------
    read_gml, generate_gml
    literal_stringizer

    Notes
    -----
    Graph attributes named 'directed', 'multigraph', 'node' or
    'edge', node attributes named 'id' or 'label', edge attributes
    named 'source' or 'target' (or 'key' if `G` is a multigraph);
    are ignored because these attribute names are used to encode the graph
    structure.

    GML files are stored using a 7-bit ASCII encoding with any extended
    ASCII characters (iso8859-1) appearing as HTML character entities.
    Without specifying a `stringizer`/`destringizer`, the code is capable of
    writing `int`/`double`/`str`/`dict`/`list` data as required by the GML
    specification.  For writing other data types, and for reading data other
    than `str` you need to explicitly supply a `stringizer`/`destringizer`.

    Note that while we allow non-standard GML to be read from a file, we make
    sure to write GML format. In particular, underscores are not allowed in
    attribute names.
    For additional documentation on the GML file format, please see the
    `GML url <https://web.archive.org/web/20190207140002/http://www.fim.uni-passau.de/index.php?id=17297&L=1>`_.

    See the module docstring :mod:`graphx.readwrite.gml` for more details.

    Examples
    --------
    >>> G = nx.path_graph(4);
    >>> nx.write_gml(G, "test.gml");

    Filenames ending in .gz or .bz2 will be compressed.

    >>> nx.write_gml(G, "test.gml.gz");
    */
    for (auto line : generate_gml(G, stringizer)) {
        path.write((line + "\n").encode("ascii"));
