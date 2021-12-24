#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <exception>

class Opts
{
public:
    virtual ~Opts() = default;
};

class Bipartite_Opts: public Opts
{
public:    
    std::vector<char> part_1;
    std::vector<char> part_2;
    ~Bipartite_Opts() {}
    Bipartite_Opts(std::vector<char> init_part_1, std::vector<char> init_part_2);
};

class Simple_Opts: public Opts
{
 public:   
    std::vector<std::string> edges;
    Simple_Opts(std::vector<std::string> init_edges);
};

class Complete_Opts: public Opts
{
public:    
    std::vector<char> vertices;
    Complete_Opts(std::vector<char> init_vertices);
};

class Weighted_Opts: public Opts
{
public:    
    std::vector<std::string> edges;
    std::vector<int> weights;
    Weighted_Opts(std::vector<std::string> init_edges, std::vector<int> init_weights);
};

class TGraph
{
public:
    virtual ~TGraph() {};
    virtual const std::string ToString() const = 0;
    virtual const std::vector<char> GetVertices() const = 0;
    virtual const std::vector<std::pair<char, char>> GetEdges() const = 0;
    virtual std::unique_ptr<TGraph> AsWeighted(int default_weight) const = 0;
};

class Weighted: public TGraph
{
    std::vector<char> vertices;
    std::vector<std::pair<char, char>> edges;
    std::vector<int> weights;
public:
    using Opts_Type = Weighted_Opts;
    Weighted(std::unique_ptr<Weighted_Opts> && opts);
    Weighted(std::vector<std::string> init_edges, std::vector<int> init_weights);
    Weighted(const Weighted &other);
    ~Weighted() {}

    std::unique_ptr<TGraph> AsWeighted(int default_weight) const;
    const std::string ToString() const; 
    const std::vector<char> GetVertices() const;
    const std::vector<std::pair<char, char>> GetEdges() const;

    friend Weighted operator+=(Weighted &graph_1, Weighted &graph_2);
    friend Weighted operator+(Weighted &graph_1, Weighted &graph_2);
    friend Weighted operator+(Weighted &graph_1, TGraph &graph_2);

    friend Weighted operator-=(Weighted &graph_1, Weighted &graph_2);
    friend Weighted operator-(Weighted &graph_1, TGraph &graph_2);

    const std::vector<int> GetWeights() const;

};

class Simple: public TGraph
{
    std::vector<char> vertices;
    std::vector<std::pair<char, char>> edges;
public:
    using Opts_Type = Simple_Opts;
    Simple(std::unique_ptr<Simple_Opts> && opts);
    Simple(std::vector<std::string> init_edges);
    Simple(const Simple &other);
    ~Simple() {}

    std::unique_ptr<TGraph> AsWeighted(int default_weight) const;
    const std::string ToString() const;
    const std::vector<char> GetVertices() const;
    const std::vector<std::pair<char, char>> GetEdges() const;

    friend Simple operator+=(Simple &graph_1, TGraph &graph_2);
    friend Simple operator+(Simple &graph_1, TGraph &graph_2);
    friend Simple operator+(Simple &graph_1, Weighted &graph_2);

    friend Simple operator-=(Simple &graph_1, TGraph &graph_2);
    friend Simple operator-(Simple &graph_1, TGraph &graph_2);
};

class Bipartite: public TGraph
{
    std::vector<char> part_1;
    std::vector<char> part_2;
public:
    using Opts_Type = Bipartite_Opts;
    Bipartite(std::unique_ptr<Bipartite_Opts> && opts);
    Bipartite(std::vector<char> p1, std::vector<char> p2);
    Bipartite(const Bipartite &other);
    ~Bipartite() {}

    std::unique_ptr<TGraph> AsWeighted(int default_weight) const;
    const std::string ToString() const;
    const std::vector<char> GetVertices() const;
    const std::vector<std::pair<char, char>> GetEdges() const;

    friend Bipartite operator+=(Bipartite &graph_1, Bipartite &graph_2);
    friend Bipartite operator+(Bipartite &graph_1, Bipartite &graph_2);
    friend Bipartite operator+(Bipartite &graph_1, Weighted &graph_2);
    friend Simple operator+(Bipartite &graph_1, TGraph &graph_2);

    friend Bipartite operator-=(Bipartite &graph_1, Bipartite &graph_2);
    friend Bipartite operator-(Bipartite &graph_1, Bipartite &graph_2);
    friend Simple operator-(Bipartite &graph_1, TGraph &graph_2);

    const std::vector<char> GetPart_1() const;
    const std::vector<char> GetPart_2() const;
};

class Complete: public TGraph
{
    std::vector<char> vertices;
public:
    using Opts_Type = Complete_Opts;
    Complete(std::unique_ptr<Complete_Opts> && opts);
    Complete(std::vector<char> init_vertices);
    Complete(const Complete &other);
    ~Complete() {}

    std::unique_ptr<TGraph> AsWeighted(int default_weight) const;
    const std::string ToString() const;
    const std::vector<char> GetVertices() const;
    const std::vector<std::pair<char, char>> GetEdges() const;

    friend Complete operator+=(Complete &graph_1, Complete &graph_2);
    friend Complete operator+(Complete &graph_1, Complete &graph_2);
    friend Simple operator+(Complete &graph_1, TGraph &graph_2);
    friend Simple operator+(Complete &graph_1, Weighted &graph_2);

    friend Complete operator-=(Complete &graph_1, Complete &graph_2);
    friend Complete operator-(Complete &graph_1, Complete &graph_2);
    friend Simple operator-(Complete &graph_1, TGraph &graph_2);
};