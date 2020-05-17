#include "nodegraph/view/viewnode.h"

namespace NodeGraph
{

ViewNode::ViewNode(Node* pModel)
    : pModelNode(pModel)
{
    /*
    for (auto& in : pModelNode->GetInputs())
    {
        ImNodes::Ez::SlotInfo slot;
        slot.kind = (int)in->GetType();
        slot.title = in->GetName().c_str();
        input_slots.push_back(slot);
    }

    for (auto& out : pModelNode->GetOutputs())
    {
        ImNodes::Ez::SlotInfo slot;
        slot.kind = (int)out->GetType();
        slot.title = out->GetName().c_str();
        output_slots.push_back(slot);
    }
    */
}

} // namespace NodeGraph