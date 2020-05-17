#pragma once

#include <map>

#include "nodegraph/model/graph.h"
#include "nodegraph/view/viewnode.h"

struct NVGcontext;

namespace NodeGraph
{

struct SliderData
{
    MUtils::NRectf channel;
    MUtils::NRectf thumb;
};

class GraphView
{
public:
    GraphView(Graph& graph, NVGcontext* vgContext)
        : m_graph(graph),
        vg(vgContext)
    {}

    void BuildNodes();
    void Show(const glm::ivec2& displaySize);
    bool ShowNode(const Node* pNode) const;


    bool DrawKnob(glm::vec2 pos, float knobSize, Pin& pin);
    SliderData DrawSlider(MUtils::NRectf pos, Pin& pin);
    void DrawButton(MUtils::NRectf pos, Pin& pin);

    MUtils::NRectf DrawNode(const MUtils::NRectf& pos, Node* pNode);

    void DrawLabel(Parameter& param, const glm::vec2& pos);

    bool CheckCapture(Parameter& param, const MUtils::NRectf& region, bool& hover);
    bool HideCursor() const { return m_hideCursor; }

private:
    enum class InputDirection { X, Y };

    void EvaluateDragDelta(Pin& pin, float delta, InputDirection dir);
    void CheckInput(Pin& param, const MUtils::NRectf& region, float rangePerDelta, bool& hover, bool& captured, InputDirection dir);

private:
    Graph& m_graph;
    std::map<Node*, std::shared_ptr<ViewNode>> mapWorldToView;
    std::map<uint32_t, Node*> mapInputOrder;

    NVGcontext* vg = nullptr;

    Parameter* m_pCaptureParam = nullptr;
    MUtils::NVec2f m_mouseStart;


    bool m_hideCursor = false;
    uint32_t m_currentInputIndex = 0;

    std::map<Parameter*, glm::vec2> m_drawLabels;

};

};