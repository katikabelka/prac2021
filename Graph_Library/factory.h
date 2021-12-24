#include <map>
#include "all_graphs.h"

class GraphFactory {
    class TImpl;
    std::unique_ptr<const TImpl> Impl;
public:
    GraphFactory();
    ~GraphFactory();
    std::unique_ptr<TGraph> Create(const std::string& type, std::unique_ptr<Opts>&& opts) const;
};