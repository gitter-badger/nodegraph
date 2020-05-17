#pragma once

#include <cassert>
#include <exception>
#include <functional>
#include <set>

#include "mutils/profile/profile.h"

#include "threadpool/threadpool.h"

#include "nodegraph/model/node.h"
#include "nodegraph/model/pin.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace NodeGraph
{

// A collection of nodes
class Graph
{
public:
    Graph();
    virtual ~Graph();

    void Destroy();

    // Use this method to create nodes and add them to the graph
    template <typename T, typename... Args>
    T* CreateNode(Args&&... args)
    {
        auto pNode = std::make_shared<T>(*this, std::forward<Args>(args)...);
        nodes.insert(pNode);
        return pNode.get();
    }

    template <class T>
    std::set<T*> Find(ctti::type_id_t type) const
    {
        std::set<T*> found;
        for (auto& pNode : nodes)
        {
            if (pNode->GetType() == type)
                found.insert(static_cast<T*>(pNode.get()));
        }
        return found;
    }

    template <class T>
    std::set<T*> Find(const std::vector<ctti::type_id_t>& nodeTypes) const
    {
        std::set<T*> found;
        for (auto& t : nodeTypes)
        {
            auto f = Find<T>(t);
            found.insert(f.begin(), f.end());
        }
        return found;
    }

    void Visit(Node& node, PinDir dir, ParameterType type, std::function<bool(Node&)> fn);

    // Get the list of pins that could be on the UI
    std::vector<Pin*> GetControlSurface() const;

    void Compute(const std::vector<Node*>& nodes, int64_t numTicks);

    const std::set<std::shared_ptr<Node>>& GetNodes() const { return nodes; }

    TPool& ThreadPool() { return m_threadPool; }

    const std::vector<Node*> GetEvalNodes() const { return evalNodes; }
    
protected:
    std::set<std::shared_ptr<Node>> nodes;
    std::vector<Node*> evalNodes;
    uint64_t currentGeneration = 1;
    TPool m_threadPool;
}; // Graph

} // namespace NodeGraph