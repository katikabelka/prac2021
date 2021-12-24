#include "all_graphs.h"


Bipartite_Opts::Bipartite_Opts(std::vector<char> init_part_1, std::vector<char> init_part_2)
{
    for (auto i: init_part_1) {
        part_1.push_back(i);
    }
    for (auto i: init_part_2) {
        part_2.push_back(i);
    }
}

Weighted_Opts::Weighted_Opts(std::vector<std::string> init_edges, std::vector<int> init_weights)
{
    for (auto i: init_edges) {
        edges.push_back(i);
    }
    for (auto i: init_weights) {
        weights.push_back(i);
    }
}

Simple_Opts::Simple_Opts(std::vector<std::string> init_edges)
{
    for (auto i: init_edges) {
        edges.push_back(i);
    }
}

Complete_Opts::Complete_Opts(std::vector<char> init_vertices)
{
    for (auto i: init_vertices) {
        vertices.push_back(i);
    }
}

Weighted::Weighted(std::unique_ptr<Weighted_Opts> && opts)
{
    auto init_edges = opts->edges;
    auto init_weights = opts->weights;

    for (auto edge: init_edges) {
        auto new_edge = std::make_pair(edge[0], edge[1]);
        edges.push_back(new_edge);

        if (std::find(vertices.begin(), vertices.end(), edge[0]) == vertices.end()) {
            vertices.push_back(edge[0]);
        }
        if (std::find(vertices.begin(), vertices.end(), edge[1]) == vertices.end()) {
            vertices.push_back(edge[1]);
        }
    }
    std::sort(vertices.begin(), vertices.end());

    for (auto weight: init_weights) {
        weights.push_back(weight);
    }
}

Weighted::Weighted(std::vector<std::string> init_edges, std::vector<int> init_weights)
{
    for (auto edge: init_edges) {
        auto new_edge = std::make_pair(edge[0], edge[1]);
        edges.push_back(new_edge);
        if (std::find(vertices.begin(), vertices.end(), edge[0]) == vertices.end()) {
            vertices.push_back(edge[0]);
        }
        if (std::find(vertices.begin(), vertices.end(), edge[1]) == vertices.end()) {
            vertices.push_back(edge[1]);
        }
    }
    std::sort(vertices.begin(), vertices.end());
    for (auto weight: init_weights) {
        weights.push_back(weight);
    }
}

Weighted::Weighted(const Weighted &other)
{
    std::vector<std::string> init_edges;
    for (auto edg: other.GetEdges()) {
        std::stringstream ss;
        ss << edg.first << edg.second;
        init_edges.push_back(ss.str());
    }
    *this = Weighted(init_edges, other.weights);
}

std::unique_ptr<TGraph> Weighted::AsWeighted(int default_weight) const
{
    std::vector<std::string> init_edges;
    for (auto edg: this->edges) {
        std::stringstream ss;
        ss << edg.first << edg.second;
        init_edges.push_back(ss.str());
    }
    return std::make_unique<Weighted> (init_edges, weights);
}

const std::string Weighted::ToString() const
{
    std::stringstream ss;
    ss << "WeightedGraph {";
    int size = edges.size();
    for (int i = 0; i < size; i++) {
        ss << edges[i].first << edges[i].second << ":" << weights[i];
        if (i != size - 1) {
            ss << ", ";
        }
    }
    ss << "}";
    return ss.str();
}

const std::vector<char> Weighted::GetVertices() const { return vertices; }

const std::vector<std::pair<char, char>> Weighted::GetEdges() const { return edges; }

const std::vector<int> Weighted::GetWeights() const { return weights; }


Weighted operator+=(Weighted &graph_1, Weighted &graph_2)
{
    auto edges_2 = graph_2.GetEdges();
    auto weights_2 = graph_2.GetWeights();
    auto len = edges_2.size();
    for (int i = 0; i < len; ++i) {
        auto reversed = std::make_pair(edges_2[i].second, edges_2[i].first);
        if (std::find(graph_1.edges.begin(), graph_1.edges.end(), edges_2[i]) == graph_1.edges.end() &&
            std::find(graph_1.edges.begin(), graph_1.edges.end(), reversed) == graph_1.edges.end()) {
            graph_1.edges.push_back(edges_2[i]);
            graph_1.weights.push_back(weights_2[i]);
        }
    }
    return graph_1;
}

Weighted operator+(Weighted &graph_1, Weighted &graph_2)
{
    Weighted new_graph = Weighted(graph_1);
    return new_graph += graph_2;
}

Weighted operator+(Weighted &graph_1, TGraph &graph_2)
{
    throw std::logic_error("Can't sum this!");
}

Weighted operator-=(Weighted &graph_1, Weighted &graph_2)
{
    auto edges_2 = graph_2.GetEdges();
    for (auto e: edges_2) {
        auto p = std::find(graph_1.edges.begin(), graph_1.edges.end(), e);
        if (p != graph_1.edges.end()) {
            graph_1.edges.erase(p);
            graph_1.weights.erase(graph_1.weights.begin() + (p - graph_1.edges.begin()));
        }
        auto reversed = std::make_pair(e.second, e.first);
        p = std::find(graph_1.edges.begin(), graph_1.edges.end(), reversed);
        if (p != graph_1.edges.end()) {
            graph_1.edges.erase(p);
            graph_1.weights.erase(graph_1.weights.begin() + (p - graph_1.edges.begin()));
        }
    }
    return graph_1;
}

Weighted operator-(Weighted &graph_1, TGraph &graph_2)
{
    auto edges_1 = graph_1.GetEdges();
    auto weights_1 = graph_1.GetWeights();
    auto edges_2 = graph_2.GetEdges();

    for (auto e: edges_2) {
        auto p = std::find(edges_1.begin(), edges_1.end(), e);
        if (p != edges_1.end()) {
            edges_1.erase(p);
            weights_1.erase(weights_1.begin() + (p - edges_1.begin()));
        }
        auto reversed = std::make_pair(e.second, e.first);
        p = std::find(edges_1.begin(), edges_1.end(), reversed);
        if (p != edges_1.end()) {
            edges_1.erase(p);
            weights_1.erase(weights_1.begin() + (p - edges_1.begin()));
        }
    }
    std::vector<std::string> new_edges;
    for (auto e: edges_1) {
        std::stringstream ss;
        ss << e.first << e.second;
        new_edges.push_back(ss.str());
    }
    return Weighted(new_edges, weights_1);
}

Simple::Simple(std::unique_ptr<Simple_Opts> && opts)
{
    auto init_edges = opts->edges;
    for (auto edge: init_edges) {
        auto new_edge = std::make_pair(edge[0], edge[1]);
        edges.push_back(new_edge);
        if (std::find(vertices.begin(), vertices.end(), edge[0]) == vertices.end()) {
            vertices.push_back(edge[0]);
        }
        if (std::find(vertices.begin(), vertices.end(), edge[1]) == vertices.end()) {
            vertices.push_back(edge[1]);
        }
    }
    std::sort(vertices.begin(), vertices.end());
}

Simple::Simple(std::vector<std::string> init_edges)
{
    for (auto edge: init_edges) {
        auto new_edge = std::make_pair(edge[0], edge[1]);
        edges.push_back(new_edge);
        if (std::find(vertices.begin(), vertices.end(), edge[0]) == vertices.end()) {
            vertices.push_back(edge[0]);
        }
        if (std::find(vertices.begin(), vertices.end(), edge[1]) == vertices.end()) {
            vertices.push_back(edge[1]);
        }
    }
    std::sort(vertices.begin(), vertices.end());
}

Simple::Simple(const Simple &other)
{
    std::vector<std::string> init_edges;
    for (auto e: other.GetEdges()) {
        std::stringstream ss;
        ss << e.first << e.second;
        init_edges.push_back(ss.str());
    }
    *this = Simple(init_edges);
}

std::unique_ptr<TGraph> Simple::AsWeighted(int default_weight) const
{
    std::vector<std::string> init_edges;
    std::vector<int> weights;
    for (auto e: edges) {
        std::stringstream ss;
        ss << e.first << e.second;
        init_edges.push_back(ss.str());
        weights.push_back(default_weight);
    }
    return std::make_unique<Weighted>(std::make_unique<Weighted_Opts> (init_edges, weights));
}


const std::string Simple::ToString() const
{
    std::stringstream ss;
    ss << "SimpleGraph {";
    for (auto &edg: edges) {
        ss << edg.first << edg.second;
        if (edg != *(edges.end()-1)) {
            ss << ", ";
        }
    }
    ss << "}";
    return ss.str();
}

const std::vector<char> Simple::GetVertices() const { return vertices; }

const std::vector<std::pair<char, char>> Simple::GetEdges() const { return edges; }

Simple operator+=(Simple &graph_1, TGraph &graph_2)
{
    auto edges_1 = graph_1.GetEdges();
    auto edges_2 = graph_2.GetEdges();
    std::vector<std::string> edges;
    auto len = edges_2.size();
    for (int i = 0; i < len; i++) {
        auto reversed = std::make_pair(edges_2[i].second, edges_2[i].first);
        if (std::find(graph_1.edges.begin(), graph_1.edges.end(), edges_2[i]) == graph_1.edges.end() && 
        std::find(graph_1.edges.begin(), graph_1.edges.end(), reversed) == graph_1.edges.end()) {
            graph_1.edges.push_back(edges_2[i]);
        }
    }
    return graph_1;
}

Simple operator+(Simple &graph_1, TGraph &graph_2)
{
    Simple new_graph = Simple(graph_1);
    return new_graph += graph_2;
}

Simple operator+(Simple &graph_1, Weighted &graph_2)
{
    throw std::logic_error("Can't sum this!");
}

Simple operator-=(Simple &graph_1, TGraph &graph_2)
{
    auto edges_2 = graph_2.GetEdges();
    for (auto e: edges_2) {
        auto p = std::find(graph_1.edges.begin(), graph_1.edges.end(), e);
        if (p != graph_1.edges.end()) {
            graph_1.edges.erase(p);
        }
        auto reversed = std::make_pair(e.second, e.first);
        p = std::find(graph_1.edges.begin(), graph_1.edges.end(), reversed);
        if (p != graph_1.edges.end()) {
            graph_1.edges.erase(p);
        }
    }
    return graph_1;
}

Simple operator-(Simple &graph_1, TGraph &graph_2)
{
    auto new_graph = Simple(graph_1);
    return new_graph -= graph_2;
}

Bipartite::Bipartite(std::unique_ptr<Bipartite_Opts> && opts)
{
    auto p1 = opts->part_1;
    for (auto v: p1) {
        part_1.push_back(v);
    }

    auto p2 = opts->part_2;
    for (auto v: p2) {
        part_2.push_back(v);
    }
}

Bipartite::Bipartite(std::vector<char> p1, std::vector<char> p2)
{
    for (auto v: p1) {
        part_1.push_back(v);
    }
    for (auto v: p2) {
        part_2.push_back(v);
    }
}

Bipartite::Bipartite(const Bipartite &other) { *this = Bipartite(other.part_1, other.part_2); }

std::unique_ptr<TGraph> Bipartite::AsWeighted(int default_weight) const
{
    std::vector<std::string> edges;
    std::vector<int> weights;
    for (auto v1: part_1) {
        for (auto v2: part_2) {
            std::stringstream ss;
            ss << v1 << v2;
            edges.push_back(ss.str());
            weights.push_back(default_weight);
        }
    }
    return std::make_unique<Weighted>(std::make_unique<Weighted_Opts> (edges, weights));
}

const std::string Bipartite::ToString() const
{
    std::stringstream ss;
    ss << "BipartiteGraph {{";
    for (auto &v1: part_1) {
        ss << v1;
        if (v1 != *(part_1.end() - 1)) {
            ss << ", ";
        }
    }
    ss << "}, {";
    for (auto &v2: part_2) {
        ss << v2;
        if (v2 != *(part_2.end() - 1)) {
            ss << ", ";
        }
    }
    ss << "}}";
    return ss.str();
}


const std::vector<char> Bipartite::GetVertices() const
{
    std::vector<char>result;
    for (auto v: part_1) {
        result.push_back(v);
    }
    for (auto v: part_2) {
        result.push_back(v);
    }
    return result;
}

const std::vector<char> Bipartite::GetPart_1() const { return part_1; }

const std::vector<char> Bipartite::GetPart_2() const { return part_2; }

const std::vector<std::pair<char, char>> Bipartite::GetEdges() const
{
    std::vector<std::pair<char, char>> edges;
    for (auto v1: part_1) {
        for (auto v2: part_2) {
            edges.push_back(std::make_pair(v1, v2));
        }
    }
    return edges;
}

Bipartite operator+=(Bipartite &graph_1, Bipartite &graph_2) 
{
    auto gr2_part_1 = graph_2.GetPart_1();
    auto gr2_part_2 = graph_2.GetPart_2();
    for (auto v: gr2_part_1) {
        if (std::find(graph_1.part_1.begin(), graph_1.part_1.end(), v) == graph_1.part_1.end()) {
            if (std::find(graph_1.part_2.begin(), graph_1.part_2.end(), v) != graph_1.part_2.end()) {
                throw std::logic_error("Can't sum this!");
            }
            graph_1.part_1.push_back(v);
        }
    }
    for (auto v: gr2_part_2) {
        if (std::find(graph_1.part_2.begin(), graph_1.part_2.end(), v) == graph_1.part_2.end()) {
            if (std::find(graph_1.part_1.begin(), graph_1.part_1.end(), v) != graph_1.part_1.end()) {
                throw std::logic_error("Can't sum this!");
            }
            graph_1.part_2.push_back(v);
        }
    }
    return graph_1;
}

Bipartite operator+(Bipartite &graph_1, Bipartite &graph_2)
{
    Bipartite sum = Bipartite(graph_1);
    return sum+= graph_2;
}

Bipartite operator+(Bipartite &graph_1, Weighted &graph_2) { throw std::logic_error("Can't sum this!"); }

Simple operator+(Bipartite &graph_1, TGraph &graph_2)
{
    auto edges_1 = graph_1.GetEdges();
    auto edges_2 = graph_2.GetEdges();
    std::vector<std::string> edges;
    for (auto e: edges_1) {
        std::stringstream ss;
        std::string s, reversedd;
        ss << e.first << e.second << " " << e.second << e.first;
        ss >> s >> reversedd;
        if (std::find(edges.begin(), edges.end(), s) == edges.end() &&
            std::find(edges.begin(), edges.end(), reversedd) == edges.end()) {
            edges.push_back(s);
        }
    }
    for (auto e: edges_2) {
        std::stringstream ss;
        std::string s, reversedd;
        ss << e.first << e.second << " " << e.second << e.first;
        ss >> s >> reversedd;
        if (std::find(edges.begin(), edges.end(), s) == edges.end() &&
            std::find(edges.begin(), edges.end(), reversedd) == edges.end()) {
            edges.push_back(s);
        }
    }
    return Simple(edges);
}

Bipartite operator-=(Bipartite &graph_1, Bipartite &graph_2)
{
    auto gr2_part_1 = graph_2.GetPart_1();
    auto gr2_part_2 = graph_2.GetPart_2();
    for (auto v: gr2_part_1) {
        auto p = std::find(graph_1.part_1.begin(), graph_1.part_1.end(), v);
        if (p != graph_1.part_1.end()) {
            graph_1.part_1.erase(p);
        }
    }
    for (auto v: gr2_part_2) {
        auto p = std::find(graph_1.part_2.begin(), graph_1.part_2.end(), v);
        if (p != graph_1.part_2.end()) {
            graph_1.part_2.erase(p);
        }
    }
    return graph_1;
}

Bipartite operator-(Bipartite &graph_1, Bipartite &graph_2)
{
    auto new_graph = Bipartite(graph_1);
    return new_graph -= graph_2;
}

Simple operator-(Bipartite &graph_1, TGraph &graph_2)
{
    auto edges_1 = graph_1.GetEdges();
    auto edges_2 = graph_2.GetEdges();
    for (auto e: edges_2) {
        auto p = std::find(edges_1.begin(), edges_1.end(), e);
        if (p != edges_1.end()) {
            edges_1.erase(p);
        }
        auto reversed = std::make_pair(e.second, e.first);
        p = std::find(edges_1.begin(), edges_1.end(), reversed);
        if (p != edges_1.end()) {
            edges_1.erase(p);
        }
    }
    std::vector<std::string> new_edges;
    for (auto e: edges_1) {
        std::stringstream ss;
        ss << e.first << e.second;
        new_edges.push_back(ss.str());
    }
    return Simple(new_edges);
}

Complete::Complete(std::unique_ptr<Complete_Opts> && opts) 
{
    auto init_vertices = opts->vertices;
    for (auto v: init_vertices) {
        vertices.push_back(v);
    }
    
}

Complete::Complete(std::vector<char> init_vertices) 
{
    for (auto v: init_vertices) {
        vertices.push_back(v);
    }
}

Complete::Complete(const Complete &other) { *this = Complete(other.GetVertices()); }


std::unique_ptr<TGraph> Complete::AsWeighted(int default_weight) const
{
    std::vector<std::string> edges;
    std::vector<int> weights;
    for (auto it1 = vertices.begin(); it1 != vertices.end(); it1++) {
        for (auto it2 = it1 + 1; it2 != vertices.end(); it2++) {
            std::stringstream ss;
            ss << *it1 << *it2;
            edges.push_back(ss.str());
            weights.push_back(default_weight);
        }
    }
    return std::make_unique<Weighted>(std::make_unique<Weighted_Opts> (edges, weights));
}

const std::string Complete::ToString() const
{
    std::stringstream ss;
    ss << "CompleteGraph {";
    for (auto &v: vertices) {
        ss << v;
        if (v != *(vertices.end() - 1))
        {
            ss << ", ";
        }
    }
    ss << "}";
    return ss.str();
}

const std::vector<char> Complete::GetVertices() const { return vertices; }

const std::vector<std::pair<char, char>> Complete::GetEdges() const
{
    std::vector<std::pair<char, char>> edges;
    for (auto it1 = vertices.begin(); it1 != vertices.end(); it1++) {
        for (auto it2 = it1 + 1; it2 != vertices.end(); it2++) {
            edges.push_back(std::make_pair(*it1, *it2));
        }
    }
    return edges;
}

Complete operator+=(Complete &graph_1, Complete &graph_2)
{
    auto verts_2 = graph_2.GetVertices();
    for (auto v: verts_2) {
        if (std::find(graph_1.vertices.begin(), graph_1.vertices.end(), v) == graph_1.vertices.end()) {
            graph_1.vertices.push_back(v);
        }
    }
    return graph_1;
}

Complete operator+(Complete &graph_1, Complete &graph_2)
{
    Complete new_graph = Complete(graph_1);
    return new_graph += graph_2;
}

Simple operator+(Complete &graph_1, TGraph &graph_2)
{
    auto edges_1 = graph_1.GetEdges();
    auto edges_2 = graph_2.GetEdges();
    std::vector<std::string> edges;
    for (auto e: edges_1) {
        std::stringstream ss;
        std::string s, reversed;
        ss << e.first << e.second << " " << e.second << e.first;
        ss >> s >> reversed;
        if (std::find(edges.begin(), edges.end(), s) == edges.end() &&
            std::find(edges.begin(), edges.end(), reversed) == edges.end()) {
            edges.push_back(s);
        }
    }
    for (auto e: edges_2) {
        std::stringstream ss;
        std::string s, reversed;
        ss << e.first << e.second << " " << e.second << e.first;
        ss >> s >> reversed;
        if (std::find(edges.begin(), edges.end(), s) == edges.end() &&
            std::find(edges.begin(), edges.end(), reversed) == edges.end()) {
            edges.push_back(s);
        }
    }
    return Simple(edges);
}

Simple operator+(Complete &graph_1, Weighted &graph_2)
{
    throw std::logic_error("Can't sum this!");
}

Complete operator-=(Complete &graph_1, Complete &graph_2)
{
    auto verts_2 = graph_2.GetVertices();
    for (auto v: verts_2) {
        auto p = std::find(graph_1.vertices.begin(), graph_1.vertices.end(), v);
        if (p != graph_1.vertices.end()) {
            graph_1.vertices.erase(p);
        }
    }
    return graph_1;
}

Complete operator-(Complete &graph_1, Complete &graph_2)
{
    auto new_graph = Complete(graph_1);
    return new_graph -= graph_2;
}

Simple operator-(Complete &graph_1, TGraph &graph_2)
{
    auto edges_1 = graph_1.GetEdges();
    auto edges_2 = graph_2.GetEdges();
    for (auto e: edges_2) {
        auto p = std::find(edges_1.begin(), edges_1.end(), e);
        if (p != edges_1.end()) {
            edges_1.erase(p);
        }
        auto reversed =std::make_pair(e.second, e.first);
        p = std::find(edges_1.begin(), edges_1.end(), reversed);
        if (p != edges_1.end()) {
            edges_1.erase(p);
        }
    }
    std::vector<std::string> init_edges;
    for (auto e: edges_1) {
        std::stringstream ss;
        ss << e.first << e.second;
        init_edges.push_back(ss.str());
    }
    return Simple(init_edges);
}