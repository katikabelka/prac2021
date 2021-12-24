#include "factory.h"
#include <typeinfo>
#include <iostream>

class GraphFactory::TImpl
{
    class ICreator {
    public:
        virtual ~ICreator(){}
        virtual std::unique_ptr<TGraph> Create(std::unique_ptr<Opts>&& opts) const = 0;
    };
    using TCreatorPtr = std::shared_ptr<ICreator>;
    using TRegisteredCreators = std::map<std::string, TCreatorPtr>;
    TRegisteredCreators RegisteredCreators;
public:
    template <class TCurrentObject>
    class TCreator : public ICreator{
        std::unique_ptr<TGraph> Create(std::unique_ptr<Opts>&& opts) const override{
            auto current_options = dynamic_cast<typename TCurrentObject::Opts_Type*>(opts.get());
            opts.release();
            try {
                std::string new_type = typeid(*current_options).name();
            } catch (std::exception &e) {
                throw std::invalid_argument("Wrong parameters");
            }
            return std::make_unique<TCurrentObject>(std::unique_ptr<typename TCurrentObject::Opts_Type>(current_options));
        }
    };

    TImpl() { RegisterAll();}

    template <typename T>
    void RegisterCreator(const std::string& type) {
        RegisteredCreators[type] = std::make_shared<TCreator<T>>();
    }
    void RegisterAll() {
        RegisterCreator<Bipartite>("bipartite");
        RegisterCreator<Complete>("complete");
        RegisterCreator<Simple>("simple");
        RegisterCreator<Weighted>("weighted");
    }
    std::unique_ptr<TGraph> CreateObject(const std::string& type, std::unique_ptr<Opts>&& opts) const {
        auto creator = RegisteredCreators.find(type);
        if (creator == RegisteredCreators.end()) {
            throw std::invalid_argument("Wrong type");
        }
        std::unique_ptr<TGraph> res;
        return creator->second->Create(std::move(opts));
    }
};

GraphFactory::GraphFactory() : Impl(std::make_unique<GraphFactory::TImpl>()) {}
GraphFactory::~GraphFactory(){}

std::unique_ptr<TGraph> GraphFactory::Create(const std::string& type, std::unique_ptr<Opts>&& opts) const
{
    return Impl->CreateObject(type, std::move(opts));
}