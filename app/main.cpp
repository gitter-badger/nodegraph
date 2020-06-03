#include <mutils/logger/logger.h>

#include <mutils/math/imgui_glm.h>
#include <mutils/ui/sdl_imgui_starter.h>

#include "config_app.h"
#include <SDL.h>
#include <nodegraph/model/graph.h>
#include <nodegraph/view/graphview.h>

#include <GL/gl3w.h>
#include <nanovg/nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg/nanovg_gl.h>

using namespace MUtils;
using namespace NodeGraph;

#undef ERROR
class TestNode : public Node
{
public:
    DECLARE_NODE(TestNode, adder);

    TestNode(Graph& graph)
        : Node(graph, "UI Test")
    {
        pSum = AddOutput("Sumf", .0f, ParameterAttributes(ParameterUI::Knob, 0.0f, 1.0f));
        pSum->GetAttributes().flags |= ParameterFlags::ReadOnly;

        pValue2 = AddInput("0-1000f", 5.0f, ParameterAttributes(ParameterUI::Knob, 0.01f, 1000.0f));
        pValue2->GetAttributes().taper = 2;

        pValue3 = AddInput("-1->+1f", .001f, ParameterAttributes(ParameterUI::Knob, -1.0f, 1.0f));

        pValue4 = AddInput("-10->+1f", .001f, ParameterAttributes(ParameterUI::Knob, -10.0f, 1.0f));

        pValue5 = AddInput("-10->10i", (int64_t)-10, ParameterAttributes(ParameterUI::Knob, (int64_t)-10, (int64_t)10));

        pValue6 = AddInput("-10->10is", (int64_t)-10, ParameterAttributes(ParameterUI::Knob, (int64_t)-10, (int64_t)10));
        pValue6->GetAttributes().step = (int64_t)4;

        pValue7 = AddInput("0->1000ie", (int64_t)0, ParameterAttributes(ParameterUI::Knob, (int64_t)0, (int64_t)1000));
        pValue7->GetAttributes().postFix = "dB";

        pValue8 = AddInput("0->1%", 0.0f, ParameterAttributes(ParameterUI::Knob, (float)0.0f, (float)1.0f));
        pValue8->GetAttributes().displayType = ParameterDisplayType::Percentage;

        pSlider = AddInput("Slider", 0.5f);
        pButton = AddInput("Button", (int64_t)0);

        pIntSlider = AddInput("Slider", (int64_t)0);
        pValue1 = AddInput("0-1f", .5f, ParameterAttributes(ParameterUI::Knob, 0.01f, 1.0f));
        pValue9 = AddInput("0-1f", .5f, ParameterAttributes(ParameterUI::Knob, 0.01f, 1.0f));

        ParameterAttributes sliderAttrib(ParameterUI::Slider, 0.0f, 1.0f);
        sliderAttrib.step = 0.25f;
        sliderAttrib.thumb = 0.25f;
        pSlider->SetAttributes(sliderAttrib);

        ParameterAttributes sliderIntAttrib(ParameterUI::Slider, (int64_t)0, (int64_t)3);
        sliderIntAttrib.step = (int64_t)1;
        sliderIntAttrib.thumb = 1 / 4.0f;
        pIntSlider->SetAttributes(sliderIntAttrib);

        ParameterAttributes buttonAttrib(ParameterUI::Button, -1ll, 3ll);
        buttonAttrib.labels = { "A", "B", "C" };
        pButton->SetAttributes(buttonAttrib);

        if (pValue2)
            pValue2->SetViewCells(NRectf(0, 0, 1, 1));
        if (pValue3)
            pValue3->SetViewCells(NRectf(1, 0, 1, 1));
        if (pValue4)
            pValue4->SetViewCells(NRectf(2, 0, 1, 1));
        if (pValue5)
            pValue5->SetViewCells(NRectf(3, 0, 1, 1));
        if (pValue6)
            pValue6->SetViewCells(NRectf(4, 0, 1, 1));
        if (pValue7)
            pValue7->SetViewCells(NRectf(5, 0, 1, 1));
        if (pValue8)
            pValue8->SetViewCells(NRectf(6, 0, 1, 1));

        // Sum
        if (pValue9)
            pValue9->SetViewCells(NRectf(4, 1, 1, 1));
        if (pValue1)
            pValue1->SetViewCells(NRectf(5, 1, 1, 1));
        if (pSum)
            pSum->SetViewCells(NRectf(3, 1, 1, 1));

        pSlider->SetViewCells(NRectf(.25f, 1, 2.5f, .5f));
        pIntSlider->SetViewCells(NRectf(.25f, 1.5, 2.5f, .5f));
        pButton->SetViewCells(NRectf(.25f, 2.0, 2.5f, .5f));
    }

    virtual void Compute() override
    {
        if (pSum)
            pSum->Set(pValue1->To<float>() + pValue9->To<float>());
    }

    Pin* pSum = nullptr;
    Pin* pValue1 = nullptr;
    Pin* pValue2 = nullptr;
    Pin* pValue3 = nullptr;
    Pin* pValue4 = nullptr;
    Pin* pValue5 = nullptr;
    Pin* pValue6 = nullptr;
    Pin* pValue7 = nullptr;
    Pin* pValue8 = nullptr;
    Pin* pValue9 = nullptr;
    Pin* pButton = nullptr;
    Pin* pSlider = nullptr;
    Pin* pIntSlider = nullptr;
};

std::vector<Node*> appNodes;

class App : public IAppStarterClient
{
public:
    App()
    {
        settings.flags |= AppStarterFlags::DockingEnable;
        settings.startSize = NVec2i(1680, 1000);
        settings.clearColor = NVec4f(.2f, .2f, .2f, 1.0f);
        settings.appName = "NodeGraph Test";

        appNodes.push_back(graph.CreateNode<EmptyNode>("Empty Node"));
        appNodes.push_back(graph.CreateNode<TestNode>());
        appNodes.push_back(graph.CreateNode<TestNode>());
        appNodes.push_back(graph.CreateNode<TestNode>());
        appNodes.push_back(graph.CreateNode<TestNode>());
    }

    // Inherited via IAppStarterClient
    virtual fs::path GetRootPath() const override
    {
        return fs::path(NODEGRAPH_ROOT);
    }

    virtual void Init() override
    {
        vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

        auto path = this->GetRootPath() / "run_tree" / "fonts" / "Roboto-Regular.ttf";
        auto font = nvgCreateFont(vg, "sans", path.string().c_str());

        spCanvas = std::make_shared<CanvasVG>(vg);
        spGraphView = std::make_shared<GraphView>(graph, *spCanvas);
        spGraphView->BuildNodes();
    }

    virtual void Update(float time, const NVec2i& displaySize) override
    {
        /*
        settings.flags &= ~AppStarterFlags::HideCursor;
        if (spGraphView)
        {
            if (spGraphView->HideCursor())
            {
                settings.flags |= AppStarterFlags::HideCursor;
            }
        }
        */
    }

    virtual void Destroy() override
    {
        fbo_destroy(fbo);
    }

    virtual void Draw(const NVec2i& displaySize) override
    {
    }

    void DrawGraph(const NVec2i& canvasSize)
    {
        if (spGraphView)
        {
            if (fbo.fbo == 0)
            {
                fbo = fbo_create();
            }
            fbo_resize(fbo, canvasSize);

            fbo_bind(fbo);

            sdl_imgui_clear(settings.clearColor);

            spGraphView->Show(canvasSize);
            graph.Compute(appNodes, 0);

            fbo_unbind(fbo, m_displaySize);
        }
    }

    void BeginCanvas(const NRectf& region)
    {
        auto mousePos = ImGui::GetIO().MousePos;

        CanvasInputState state;
        state.mousePos = NVec2f(mousePos.x - region.Left(), mousePos.y - region.Top());
        for (uint32_t i = 0; i < MOUSE_MAX; i++)
        {
            state.buttonClicked[i] = ImGui::GetIO().MouseClicked[i];
            state.buttonReleased[i] = ImGui::GetIO().MouseReleased[i];
            state.buttonDown[i] = ImGui::GetIO().MouseDown[i];
        }
        state.canCapture = ImGui::GetIO().WantCaptureMouse;
        state.mouseDelta = ImGui::GetIO().MouseDelta;
        state.dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        state.wheelDelta = ImGui::GetIO().MouseWheel;
        state.resetDrag = false;
        state.captured = false;

        spCanvas->Update(region.Size(), state);
    }

    void EndCanvas()
    {
        ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = (spCanvas->GetInputState().captured);
        if (spCanvas->GetInputState().resetDrag)
        {
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
        }
    }

    virtual void DrawGUI(const NVec2i& displaySize) override
    {
        m_displaySize = displaySize;

        ImGui::Begin("Canvas");

        ImVec2 pos = ImGui::GetCursorScreenPos();
        NRectf region = NRectf(pos.x, pos.y, ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
        BeginCanvas(region);

        DrawGraph(region.Size());

        ImGui::Image(*(ImTextureID*)&fbo.fboTexture, ImVec2(region.Width(), region.Height()), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();

        EndCanvas();

    }

    virtual AppStarterSettings& GetSettings() override
    {
        return settings;
    }

private:
    std::shared_ptr<NodeGraph::GraphView> spGraphView;
    std::shared_ptr<NodeGraph::Canvas> spCanvas;
    NodeGraph::Graph graph;
    AppStarterSettings settings;
    NVGcontext* vg = nullptr;
    MUtils::AppFBO fbo;
    NVec2i m_displaySize = 0;
};

App theApp;

// Main code
int main(int args, char** ppArgs)
{
    return sdl_imgui_start(args, ppArgs, &theApp);
}

/*

        */
