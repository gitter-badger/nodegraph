#pragma once

#include <glm/glm.hpp>

#include "nodegraph/model/node.h"
#include "nodegraph/model/pin.h"

namespace NodeGraph
{

class ViewNode
{
public:
    explicit ViewNode(Node* pModel);

    Node* pModelNode = nullptr;
    bool selected = false;
    glm::vec2 pos;
};

} // namespace NodeGraph
