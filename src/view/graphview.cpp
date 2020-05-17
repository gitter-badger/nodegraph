#include <map>

#include "nodegraph/view/graphview.h"
#include "nodegraph/view/viewnode.h"

#include <imgui/imgui.h>
#include <nanovg/nanovg.h>

#include <magic_enum/magic_enum.hpp>
#include <mutils/logger/logger.h>

#include <fmt/format.h>

using namespace MUtils;

namespace
{
glm::vec4 node_Color(.3f, .3f, .3f, 1.0f);
glm::vec4 node_TitleColor(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 node_TitleBGColor(0.4f, .4f, 0.4f, 1.0f);
glm::vec4 node_buttonTextColor(0.15f, .15f, 0.15f, 1.0f);

glm::vec4 node_shadowColor(0.1f, 0.1f, 0.1f, .5f);

float node_shadowSize = 2.0f;
float node_borderRadius = 7.0f;
float node_borderPad = 4.0f;
float node_buttonPad = 2.0f;
float node_pinPad = 4.0f;
float node_titleFontSize = 17.0f * 1.5f;
float node_titleHeight = node_titleFontSize + node_borderPad * 4.0f;
float node_gridScale = 125.0f;
float node_titleBorder = node_borderPad * 2.0f;
float node_labelPad = 6.0f;
} // namespace

namespace NodeGraph
{

bool GraphView::ShowNode(const Node* pNode) const
{
    if (pNode->IsHidden())
        return false;

    // Check for things to show
    for (auto& in : pNode->GetInputs())
    {
        if (!in->GetViewCells().Empty())
        {
            return true;
        }
    }
    for (auto& in : pNode->GetOutputs())
    {
        if (!in->GetViewCells().Empty())
        {
            return true;
        }
    }
    if (!pNode->GetCustomViewCells().Empty())
        return true;

    return false;
}

void GraphView::BuildNodes()
{
    auto ins = m_graph.GetEvalNodes();
    for (auto& pNode : ins)
    {
        if (mapWorldToView.find(pNode) == mapWorldToView.end())
        {
            if (ShowNode(pNode))
            {
                auto spViewNode = std::make_shared<ViewNode>(pNode);
                spViewNode->pos = glm::vec2(50, 50);

                mapWorldToView[pNode] = spViewNode;
                mapInputOrder[m_currentInputIndex++] = pNode;
            }
        }
    }
}

/*
            mouseOn = true;

            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
        bool mouseOn = false;
           if (ImGui::GetMousePos().x > (pos.x - knobSize * .5)&&
            ImGui::GetMousePos().x < (pos.x + knobSize * .5) &&
            ImGui::GetMousePos().y > (pos.y - knobSize * .5) &&
            ImGui::GetMousePos().y < (pos.y + knobSize * .5))
        {
        */

bool GraphView::CheckCapture(Parameter& param, const NRectf& region, bool& hover)
{
    auto pos = ImGui::GetMousePos();
    bool overParam = region.Contains(NVec2f(pos.x, pos.y));

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        m_pCaptureParam = nullptr;
    }

    if (m_pCaptureParam == nullptr)
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            if (overParam)
            {
                m_pCaptureParam = &param;
                m_mouseStart = NVec2f(pos.x, pos.y);
            }
        }
    }

    if ((m_pCaptureParam == &param) || (overParam && m_pCaptureParam == nullptr))
    {
        hover = true;
        if (param.GetAttributes().flags & ParameterFlags::ReadOnly)
        {
            m_pCaptureParam = nullptr;
        }
    }
    else
    {
        hover = false;
    }

    m_hideCursor = m_pCaptureParam != nullptr;
    return m_pCaptureParam == &param;
}

void GraphView::EvaluateDragDelta(Pin& param, float rangePerDelta, InputDirection dir)
{
    const auto& attrib = param.GetAttributes();
    const float fMin = 0.0f;
    const float fMax = 1.0f;
    const float fRange = fMax - fMin;
    float fCurrentVal = (float)param.Normalized();

    auto d = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
    float delta = (dir == InputDirection::X ? d.x : d.y);

    float fStep = (float)param.NormalizedStep();
    if (param.GetType() == ParameterType::Int64)
    {
        if (fStep == 0.0f)
        {
            fStep = (float)std::abs(1.0 / (param.GetAttributes().max.To<double>() - param.GetAttributes().min.To<double>()));
            fStep += std::numeric_limits<float>::epsilon();
        }
    }

    if (std::fabs(delta) > 0)
    {
        auto fNew = fCurrentVal + (delta * rangePerDelta);

        bool resetDrag = false;
        if (fStep != 0.0f)
        {
            if (std::fabs(fNew - fCurrentVal) < fStep)
            {
                return;
            }
        }

        if (fNew != fCurrentVal)
        {
            param.SetFromNormalized(fNew);
            resetDrag = true;
        }

        if (resetDrag)
        {
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
        }
    }
}

void GraphView::CheckInput(Pin& param, const NRectf& region, float rangePerDelta, bool& hover, bool& captured, InputDirection dir)
{
    const auto& attrib = param.GetAttributes();
    if (param.GetSource() == nullptr)
    {
        captured = CheckCapture(param, region, hover);
        if (captured)
        {
            EvaluateDragDelta(param, rangePerDelta, dir);
        }
    }
}

void GraphView::DrawLabel(Parameter& param, const glm::vec2& pos)
{
    glm::vec4 colorLabel(0.30f, 0.30f, 0.30f, 1.0f);
    glm::vec4 channelColor(0.38f, 0.38f, 0.38f, 1.0f);
    glm::vec4 fontColor(.95f, .95f, .95f, 1.0f);
    std::string val;

    if (param.GetType() == ParameterType::Float || param.GetType() == ParameterType::Double)
    {
        // Convert to 100% if necessary
        float fVal = param.To<float>();
        if (param.GetAttributes().displayType == ParameterDisplayType::Percentage && param.GetAttributes().max.To<float>() <= 1.0f)
        {
            fVal *= 100.0f;
            val = std::to_string((int)fVal);
        }
        else
        {
            val = fmt::format("{:.{}f}", fVal, 2);
        }
    }
    else
    {
        val = std::to_string(param.To<int64_t>());
    }

    switch (param.GetAttributes().displayType)
    {
    case ParameterDisplayType::Percentage:
        val += "%";
        break;
    case ParameterDisplayType::Custom:
        val += param.GetAttributes().postFix;
    default:
        break;
    }

    NRectf rcBounds;
    float bounds[4];
    nvgTextBounds(vg, pos.x, pos.y, val.c_str(), nullptr, &bounds[0]);
    rcBounds = NRectf(bounds[0], bounds[1], bounds[2] - bounds[0], bounds[3] - bounds[1]);
    rcBounds.Adjust(-node_labelPad, -node_labelPad, node_labelPad, node_labelPad);

    auto rcShadow = rcBounds;
    rcShadow.Adjust(-node_shadowSize, -node_shadowSize, node_shadowSize, node_shadowSize);
    nvgBeginPath(vg);
    nvgFillColor(vg, nvgRGBAf(node_shadowColor.x, node_shadowColor.y, node_shadowColor.z, node_shadowColor.w));
    nvgRect(vg, rcShadow.Left(), rcShadow.Top(), rcShadow.Width(), rcShadow.Height());
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgFillColor(vg, nvgRGBAf(colorLabel.x, colorLabel.y, colorLabel.z, colorLabel.w));
    nvgRect(vg, rcBounds.Left(), rcBounds.Top(), rcBounds.Width(), rcBounds.Height());
    nvgFill(vg);

    nvgFillColor(vg, nvgRGBAf(fontColor.x, fontColor.y, fontColor.z, fontColor.w));
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgText(vg, bounds[0], bounds[1], val.c_str(), nullptr);
}

bool GraphView::DrawKnob(glm::vec2 pos, float knobSize, Pin& param)
{
    glm::vec4 color(0.45f, 0.45f, 0.45f, 1.0f);
    glm::vec4 colorLabel(0.35f, 0.35f, 0.35f, 1.0f);
    glm::vec4 colorHL(0.68f, 0.68f, 0.68f, 1.0f);
    glm::vec4 channelColor(0.38f, 0.38f, 0.38f, 1.0f);
    glm::vec4 shadowColor(0.1f, 0.1f, 0.1f, .5f);
    glm::vec4 channelHLColor(0.98f, 0.48f, 0.28f, 1.0f);
    glm::vec4 channelHighColor(0.98f, 0.10f, 0.10f, 1.0f);
    glm::vec4 markColor(.9f, .9f, 0.9f, 1.0f);
    glm::vec4 markHLColor(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 fontColor(.95f, .95f, .95f, 1.0f);
    float channelWidth = 5.0f;
    float channelGap = 10;
    float fontSize = 24.0f * (knobSize / 120.0f);

    auto& label = param.GetName();
    auto& attrib = param.GetAttributes();

    // Normalized value 0->1
    float fCurrentVal = (float)param.Normalized();

    // Knob is always normalized 0->1
    float fMin = 0.0f;
    float fMax = 1.0f;
    float fOrigin = (float)param.NormalizedOrigin();

    // TODO: Figure out delta amount based on canvas size?
    float rangePerDelta = -(1.0f / 300.0f);

    if (param.GetType() == ParameterType::Int64)
    {
        // Make integer steps a bit more fluid
        rangePerDelta /= 2.0f;
    }

    auto region = NRectf(pos.x - knobSize * .5f, pos.y - knobSize * .5f, knobSize, knobSize);

    bool hover = false;
    bool captured = false;
    CheckInput(param, region, rangePerDelta, hover, captured, InputDirection::Y);

    knobSize *= .5f;
    knobSize -= (channelGap + std::ceil(channelWidth * .5f));

    // How far the marker notch is from the center of the button
    float markerInset = knobSize * .25f;

    // Extra space under the button for the font
    float fontExtra = (fontSize * 0.5f);
    knobSize -= fontExtra;
    pos.y -= std::floor(fontExtra) - 1.0f;

    // The degree ranges corrected for NVG origin which is +90 degrees from top
    float startArc = -180.0f - 60.0f;
    float endArc = 60.0f;

    // The full range
    float arcRange = (endArc - startArc);

    // Figure out where the position is on the arc
    float ratioPos = fabs((std::clamp(fCurrentVal, 0.0f, 1.0f) - fMin) / (fMax - fMin));
    auto posArc = startArc + arcRange * ratioPos;

    // Figure out where the origin is on the arc
    float ratioOrigin = fabs((fOrigin - fMin) / (fMax - fMin));
    auto posArcBegin = startArc + arcRange * ratioOrigin;

    // Knob surrounding shadow; a filled circle behind it
    nvgBeginPath(vg);
    nvgCircle(vg, pos.x, pos.y, knobSize + node_shadowSize);
    nvgFillColor(vg, nvgRGBAf(shadowColor.x, shadowColor.y, shadowColor.z, shadowColor.w));
    nvgFill(vg);

    if (param.GetAttributes().flags & ParameterFlags::ReadOnly)
    {
        color.a = .6f;
        colorHL.a = .6f;
        markColor.a = .6f;
        channelHLColor.a = .6f;
    }
    else if (hover || captured)
    {
        markColor = markHLColor;
        color = colorHL;
    }

    nvgBeginPath(vg);
    nvgCircle(vg, pos.x, pos.y, knobSize);
    auto bg = nvgLinearGradient(vg, pos.x, pos.y + knobSize, pos.x, pos.y - knobSize, nvgRGBAf(color.x, color.y, color.z, color.w), nvgRGBAf(colorHL.x, colorHL.y, colorHL.z, colorHL.w));
    nvgFillPaint(vg, bg);
    nvgFill(vg);

    // the notch on the button/indicator
    auto markerAngle = nvgDegToRad(posArc + startArc - 120.0f);
    nvgBeginPath(vg);
    nvgMoveTo(vg, pos.x + (std::cos(markerAngle) * markerInset), pos.y + (std::sin(markerAngle) * markerInset));
    nvgLineTo(vg, pos.x + (std::cos(markerAngle) * (knobSize - node_shadowSize)), pos.y + (std::sin(markerAngle) * (knobSize - node_shadowSize)));
    nvgStrokeWidth(vg, channelWidth);
    nvgStrokeColor(vg, nvgRGBAf(shadowColor.x, shadowColor.y, shadowColor.z, shadowColor.w));
    nvgStroke(vg);
    nvgStrokeWidth(vg, channelWidth - node_shadowSize);
    nvgStrokeColor(vg, nvgRGBAf(markColor.x, markColor.y, markColor.z, markColor.w));
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgStrokeColor(vg, nvgRGBAf(channelColor.x, channelColor.y, channelColor.z, channelColor.w));
    nvgArc(vg, pos.x, pos.y, knobSize + channelGap, nvgDegToRad(startArc), nvgDegToRad(60.0f), NVG_CW);
    nvgStroke(vg);

    // Cover the shortest arc between the 2 points
    if (posArcBegin > posArc)
    {
        std::swap(posArcBegin, posArc);
    }

    nvgBeginPath(vg);
    nvgStrokeColor(vg, nvgRGBAf(channelHLColor.x, channelHLColor.y, channelHLColor.z, channelHLColor.w));
    nvgArc(vg, pos.x, pos.y, knobSize + channelGap, nvgDegToRad(posArcBegin), nvgDegToRad(posArc), NVG_CW);
    nvgStroke(vg);

    if (fCurrentVal > (fMax + std::numeric_limits<float>::epsilon()))
    {
        nvgBeginPath(vg);
        nvgStrokeColor(vg, nvgRGBAf(channelHighColor.x, channelHighColor.y, channelHighColor.z, channelHighColor.w));
        nvgArc(vg, pos.x, pos.y, knobSize + channelGap, nvgDegToRad(endArc - 10), nvgDegToRad(endArc), NVG_CW);
        nvgStroke(vg);
    }
    else if (fCurrentVal < (fMin - std::numeric_limits<float>::epsilon()))
    {
        nvgBeginPath(vg);
        nvgStrokeColor(vg, nvgRGBAf(channelHighColor.x, channelHighColor.y, channelHighColor.z, channelHighColor.w));
        nvgArc(vg, pos.x, pos.y, knobSize + channelGap, nvgDegToRad(startArc), nvgDegToRad(startArc + 10), NVG_CW);
        nvgStroke(vg);
    }

    nvgFontSize(vg, fontSize);
    nvgFontFace(vg, "sans");
    nvgFillColor(vg, nvgRGBAf(fontColor.x, fontColor.y, fontColor.z, fontColor.w));
    nvgTextAlign(vg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
    nvgText(vg, pos.x, pos.y + knobSize + channelGap + fontSize * .5f + 2.0f, label.c_str(), nullptr);

    if ((captured || hover) && (param.GetAttributes().displayType != ParameterDisplayType::None))
    {
        m_drawLabels[&param] = glm::vec2(pos.x, pos.y - knobSize * 2.0f);
    }
    return false;
}

SliderData GraphView::DrawSlider(NRectf region, Pin& param)
{
    glm::vec4 color(0.30f, 0.30f, 0.30f, 1.0f);
    glm::vec4 colorHL(0.35f, 0.35f, 0.35f, 1.0f);
    glm::vec4 channelColor(0.18f, 0.18f, 0.18f, 1.0f);
    glm::vec4 shadowColor(0.8f, 0.8f, 0.8f, .5f);
    glm::vec4 channelHLColor(0.98f, 0.48f, 0.28f, 1.0f);
    glm::vec4 channelHighColor(0.98f, 0.10f, 0.10f, 1.0f);
    glm::vec4 markColor(.55f, .55f, 0.55f, 1.0f);
    glm::vec4 markHLColor(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 fontColor(.8f, .8f, .8f, 1.0f);

    auto& attrib = param.GetAttributes();

    float fCurrentVal = param.To<float>();
    float fMin = attrib.min.To<float>();
    float fMax = attrib.max.To<float>();
    float fOrigin = attrib.origin.To<float>();
    float fThumb = attrib.thumb.To<float>();
    float fRange = fMax - fMin;

    fThumb = std::clamp(fThumb, fRange * .1f, .9f);

    // Draw the shadow
    nvgBeginPath(vg);
    nvgRoundedRect(vg, region.Left(), region.Top(), region.Width(), region.Height(), node_borderRadius);
    nvgFillColor(vg, nvgRGBAf(shadowColor.x, shadowColor.y, shadowColor.z, shadowColor.w));
    nvgFill(vg);

    // Now we are at the contents
    region.Adjust(node_shadowSize, node_shadowSize, -node_shadowSize, -node_shadowSize);

    // Draw the interior
    nvgBeginPath(vg);
    nvgRoundedRect(vg, region.Left(), region.Top(), region.Width(), region.Height(), node_borderRadius);
    auto bg = nvgLinearGradient(vg, region.Left(), region.Bottom(), region.Left(), region.Top(), nvgRGBAf(color.x, color.y, color.z, color.w), nvgRGBAf(colorHL.x, colorHL.y, colorHL.z, colorHL.w));
    nvgFillPaint(vg, bg);
    nvgFill(vg);

    SliderData ret;

    region.Adjust(node_borderPad, node_borderPad, -node_borderPad, -node_borderPad);
    ret.channel = region;

    float fThumbWidth = region.Width() * fThumb;
    float fRegionWidthNoThumb = region.Width() - fThumbWidth;

    // Clamp to sensible range
    auto fVal = std::clamp(fCurrentVal, fMin, fMax);

    // Clamp origin too
    fOrigin = std::clamp(fOrigin, fMin, fMax);

    // Figure out where the position is
    float ratioPos = fabs((fVal - fMin) / (fMax - fMin));

    // Figure out where the origin is on the arc
    float ratioOrigin = fabs((fOrigin - fMin) / (fMax - fMin));

    NRectf thumbRect = NRectf(region.Left() + ratioPos * fRegionWidthNoThumb,
        region.Top(),
        fThumbWidth,
        region.Height());

    bool hover = false;
    bool captured = false;
    float rangePerPixel = fRange / fRegionWidthNoThumb;
    CheckInput(param, thumbRect, rangePerPixel, hover, captured, InputDirection::X);

    if (hover || captured)
    {
        markColor += glm::vec4(.1f, .1f, .1f, 0.0f);
    }

    // Draw the thumb
    nvgBeginPath(vg);
    nvgRoundedRect(vg, thumbRect.Left(), thumbRect.Top(), thumbRect.Width(), thumbRect.Height(), node_borderRadius);
    nvgFillColor(vg, nvgRGBAf(markColor.x, markColor.y, markColor.z, markColor.w));
    nvgFill(vg);

    ret.thumb = thumbRect;

    return ret;
};

void GraphView::DrawButton(NRectf region, Pin& param)
{
    glm::vec4 color(0.30f, 0.30f, 0.30f, 1.0f);
    glm::vec4 colorHL(0.35f, 0.35f, 0.35f, 1.0f);
    glm::vec4 channelColor(0.18f, 0.18f, 0.18f, 1.0f);
    glm::vec4 shadowColor(0.25f, 0.25f, 0.25f, 1.0f);
    glm::vec4 channelHLColor(0.98f, 0.48f, 0.18f, 1.0f);
    glm::vec4 channelHighColor(0.98f, 0.10f, 0.10f, 1.0f);
    glm::vec4 markColor(.55f, .55f, 0.55f, 1.0f);
    glm::vec4 markHLColor(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 fontColor(.8f, .8f, .8f, 1.0f);

    auto& attrib = param.GetAttributes();

    float fMin = attrib.min.To<float>();
    float fMax = attrib.max.To<float>();
    float fRange = fMax - fMin;

    // Draw the shadow
    nvgBeginPath(vg);
    nvgRoundedRect(vg, region.Left(), region.Top(), region.Width(), region.Height(), node_borderRadius);
    nvgFillColor(vg, nvgRGBAf(shadowColor.x, shadowColor.y, shadowColor.z, shadowColor.w));
    nvgFill(vg);

    // Now we are at the contents
    region.Adjust(node_shadowSize, node_shadowSize, -node_shadowSize, -node_shadowSize);

    auto currentButton = param.To<int64_t>();
    auto numButtons = attrib.max.To<int64_t>();
    bool canDisable = attrib.min.To<int64_t>() < 0 ? true : false;

    float buttonWidth = region.Width() / numButtons;
    buttonWidth -= node_buttonPad;

    auto mousePos = ImGui::GetMousePos();

    for (int i = 0; i < numButtons; i++)
    {
        auto buttonRegion = NRectf(region.Left() + i * (buttonWidth + node_buttonPad), region.Top(), buttonWidth, region.Height());

        bool overButton = buttonRegion.Contains(NVec2f(mousePos.x, mousePos.y));
        if (overButton && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            if (canDisable && currentButton == i)
            {
                currentButton = -1;
            }
            else
            {
                currentButton = i;
            }
            param.SetFrom<int64_t>(currentButton);
        }

        auto buttonColor = markColor;
        if (i == currentButton)
        {
            buttonColor = channelHLColor;
        }
        auto buttonHLColor = buttonColor + glm::vec4(.05, .05, .05, 0.0f);

        if (overButton)
        {
            buttonColor += glm::vec4(.05f, .05f, .05f, 0.0f);
            buttonHLColor += glm::vec4(.05f, .05f, .05f, 0.0f);
        }

        nvgBeginPath(vg);
        if (numButtons == 1)
        {
            buttonRegion.Adjust(0, 0, 1, 0);
            nvgRoundedRect(vg, buttonRegion.Left(), buttonRegion.Top(), buttonRegion.Width(), buttonRegion.Height(), node_borderRadius);
        }
        else
        {
            if (i == 0)
            {
                nvgRoundedRectVarying(vg, buttonRegion.Left(), buttonRegion.Top(), buttonRegion.Width(), buttonRegion.Height(), node_borderRadius, 0.0f, 0.0f, node_borderRadius);
            }
            else if (i == numButtons - 1)
            {
                buttonRegion.Adjust(0, 0, 1, 0);
                nvgRoundedRectVarying(vg, buttonRegion.Left(), buttonRegion.Top(), buttonRegion.Width(), buttonRegion.Height(), 0.0f, node_borderRadius, node_borderRadius, 0.0f);
            }
            else
            {
                nvgRect(vg, buttonRegion.Left(), buttonRegion.Top(), buttonRegion.Width(), buttonRegion.Height());
            }
        }
        auto bg = nvgLinearGradient(vg, buttonRegion.Left(), buttonRegion.Bottom(), buttonRegion.Left(), buttonRegion.Top(), nvgRGBAf(buttonColor.x, buttonColor.y, buttonColor.z, buttonColor.w), nvgRGBAf(buttonHLColor.x, buttonHLColor.y, buttonHLColor.z, buttonHLColor.w));
        nvgFillPaint(vg, bg);
        nvgFill(vg);

        if (attrib.labels.size() > i)
        {
            nvgTextAlign(vg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
            nvgFontSize(vg, buttonRegion.Height() * .5f);
            nvgFontFace(vg, "sans");
            nvgFillColor(vg, nvgRGBAf(node_buttonTextColor.x, node_buttonTextColor.y, node_buttonTextColor.z, node_buttonTextColor.w));
            nvgText(vg, buttonRegion.Center().x, buttonRegion.Center().y + 1, attrib.labels[i].c_str(), nullptr);
        }
    }
}

NRectf GraphView::DrawNode(const NRectf& pos, Node* pNode)
{
    nvgBeginPath(vg);
    nvgRoundedRect(vg, pos.Left(), pos.Top(), pos.Width(), pos.Height(), node_borderRadius);
    nvgFillColor(vg, nvgRGBAf(node_Color.x, node_Color.y, node_Color.z, node_Color.w));
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgRoundedRect(vg, pos.Left() + node_titleBorder, pos.Top() + node_titleBorder, pos.Width() - node_titleBorder * 2.0f, node_titleHeight, node_borderRadius);
    nvgFillColor(vg, nvgRGBAf(node_TitleBGColor.x, node_TitleBGColor.y, node_TitleBGColor.z, node_TitleBGColor.w));
    nvgFill(vg);

    nvgFontSize(vg, node_titleFontSize);
    nvgFontFace(vg, "sans");
    nvgFillColor(vg, nvgRGBAf(node_TitleColor.x, node_TitleColor.y, node_TitleColor.z, node_TitleColor.w));
    nvgTextAlign(vg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
    nvgText(vg, pos.Left() + pos.Width() / 2.0f, pos.Top() + node_titleBorder + node_titleHeight * .5f, pNode->GetName().c_str(), nullptr);

    auto contentRect = NRectf(pos.Left() + node_borderPad, pos.Top() + node_titleBorder + node_titleHeight + node_borderPad, pos.Width() - (node_borderPad * 2), pos.Height() - node_titleHeight - (node_titleBorder * 2.0f) - node_borderPad);

    /* Content Fill
    nvgBeginPath(vg);
    nvgRoundedRect(vg, contentRect.Left(), contentRect.Top(), contentRect.Width(), contentRect.Height(), node_borderRadius);
    nvgFillColor(vg, nvgRGBAf(node_TitleBGColor.x, node_TitleBGColor.y, node_TitleBGColor.z, node_TitleBGColor.w));
    nvgFill(vg);
    */
    return contentRect;
}

void GraphView::Show(const glm::ivec2& displaySize)
{
    BuildNodes();

    nvgBeginFrame(vg, float(displaySize.x), float(displaySize.y), 1.0f);

    glm::vec2 currentPos(10.0f, 10.0f);
    glm::vec4 nodeColor(.5f, .5f, .5f, 1.0f);
    glm::vec4 pinBGColor(.2f, .2f, .2f, 1.0f);
    glm::vec4 nodeTitleBGColor(0.3f, .3f, 0.3f, 1.0f);
    float nodeGap = 10.0f;

    float maxHeightNode = 0.0f;

    m_drawLabels.clear();

    for (auto& [id, pWorld] : mapInputOrder)
    {
        auto pView = mapWorldToView[pWorld];

        NVec2f gridSize(0);

        auto pins = pWorld->GetInputs();
        pins.insert(pins.end(), pWorld->GetOutputs().begin(), pWorld->GetOutputs().end());

        for (auto& pInput : pins)
        {
            if (pInput->GetViewCells().Empty())
                continue;
            gridSize.x = std::max(gridSize.x, pInput->GetViewCells().Right());
            gridSize.y = std::max(gridSize.y, pInput->GetViewCells().Bottom());
        }

        // Account for custom
        auto custom = pWorld->GetCustomViewCells();
        gridSize.x = std::max(gridSize.x, custom.Right());
        gridSize.y = std::max(gridSize.y, custom.Bottom());

        gridSize.x = std::max(1.0f, gridSize.x);
        gridSize.y = std::max(1.0f, gridSize.y);

        gridSize.x *= pWorld->GetGridScale().x;
        gridSize.y *= pWorld->GetGridScale().y;

        glm::vec2 nodeSize;
        nodeSize.x = gridSize.x * node_gridScale;
        nodeSize.y = (gridSize.y * node_gridScale) + node_titleHeight + node_titleBorder;

        if ((currentPos.x + nodeSize.x) > displaySize.x)
        {
            currentPos.x = 10.0f;
            currentPos.y += maxHeightNode;
            maxHeightNode = 0.0f;
        }

        auto contentRect = DrawNode(NRectf(currentPos.x, currentPos.y, nodeSize.x, nodeSize.y), pWorld);

        auto cellSize = contentRect.Size() / gridSize;

        for (auto& pInput : pins)
        {
            if (pInput->GetViewCells().Empty())
                continue;

            auto pinGrid = pInput->GetViewCells();
            pinGrid.topLeftPx.x *= pWorld->GetGridScale().x;
            pinGrid.topLeftPx.y *= pWorld->GetGridScale().y;
            pinGrid.bottomRightPx.x *= pWorld->GetGridScale().x;
            pinGrid.bottomRightPx.y *= pWorld->GetGridScale().y;

            auto pinCell = NRectf(contentRect.Left() + (pinGrid.Left() * cellSize.x),
                contentRect.Top() + (pinGrid.Top() * cellSize.y),
                cellSize.x * pinGrid.Width(),
                cellSize.y * pinGrid.Height());
            pinCell.Adjust(node_pinPad, node_pinPad, -node_pinPad, /*-node_borderPad*/ 0.0f);

            //Backpad
            /*
            nvgBeginPath(vg);
            nvgRoundedRect(vg, pinCell.Left(), pinCell.Top(), pinCell.Width(), pinCell.Height(), node_borderRadius);
            nvgFillColor(vg, nvgRGBAf(pinBGColor.x, pinBGColor.y, pinBGColor.z, pinBGColor.w));
            nvgFill(vg);
            */

            if (pInput->GetAttributes().ui == ParameterUI::Knob)
            {
                DrawKnob(glm::vec2(pinCell.Center().x, pinCell.Center().y), std::min(pinCell.Width(), pinCell.Height()) - node_pinPad * 2.0f, *pInput);
            }
            else if (pInput->GetAttributes().ui == ParameterUI::Slider)
            {
                pinCell.Adjust(node_pinPad, node_pinPad, -node_pinPad, -node_pinPad);
                DrawSlider(pinCell, *pInput);
            }
            else if (pInput->GetAttributes().ui == ParameterUI::Button)
            {
                pinCell.Adjust(node_pinPad, node_pinPad, -node_pinPad, -node_pinPad);
                DrawButton(pinCell, *pInput);
            }
            else if (pInput->GetAttributes().ui == ParameterUI::Custom)
            {
                pinCell.Adjust(node_pinPad, node_pinPad, -node_pinPad, -node_pinPad);
                pWorld->DrawCustomPin(*this, vg, pinCell, *pInput);
            }
        }

        if (!custom.Empty())
        {
            custom.topLeftPx.x *= pWorld->GetGridScale().x;
            custom.topLeftPx.y *= pWorld->GetGridScale().y;
            custom.bottomRightPx.x *= pWorld->GetGridScale().x;
            custom.bottomRightPx.y *= pWorld->GetGridScale().y;
            auto cell = NRectf(contentRect.Left() + (custom.Left() * cellSize.x),
                contentRect.Top() + (custom.Top() * cellSize.y),
                cellSize.x * custom.Width(),
                cellSize.y * custom.Height());
            cell.Adjust(node_borderPad, node_borderPad, -node_borderPad, /*-node_borderPad*/ 0.0f);

            nvgBeginPath(vg);
            nvgRoundedRect(vg, cell.Left(), cell.Top(), cell.Width(), cell.Height(), node_borderRadius);
            nvgFillColor(vg, nvgRGBAf(pinBGColor.x, pinBGColor.y, pinBGColor.z, pinBGColor.w));
            nvgFill(vg);

            pWorld->DrawCustom(*this, vg, cell);
        }

        maxHeightNode = std::max(maxHeightNode, nodeSize.y + node_borderPad * 2.0f);
        currentPos.x += nodeSize.x + node_borderPad * 2.0f;
    }

    for (auto& [param, pos] : m_drawLabels)
    {
        DrawLabel(*param, pos);
    }

    nvgEndFrame(vg);

    /*if (ImGui::Begin("ImNodes", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
    {

        ImGui::End();
    }
    */

    /*
    // Canvas must be created after ImGui initializes, because constructor accesses ImGui style to configure default colors.
    static ImNodes::CanvasState canvas{};

    // I think it is probably better to just keep node * and copy everything else into the view.
    // For now, use a lock to gain access to the nodes
    std::lock_guard<MUtilsLockableBase(std::mutex)> guard(m_graph.GetMutex());

    const ImGuiStyle& style = ImGui::GetStyle();
    if (ImGui::Begin("ImNodes", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
    {
        // We probably need to keep some state, like positions of nodes/slots for rendering connections.
        ImNodes::BeginCanvas(&canvas);

        for (auto it = m_viewNodes.begin(); it != m_viewNodes.end();)
        {
            auto node = (*it).get();

            // Start rendering node
            if (ImNodes::Ez::BeginNode(node, node->pModelNode->GetName().c_str(), &node->pos, &node->selected))
            {
                // Render input nodes first (order is important)
                ImNodes::Ez::InputSlots(node->input_slots.data(), (int)node->input_slots.size());

                // Custom node content may go here
                /*float fValue = 1.0f;
                ImGui::InputFloat("Level", &fValue );
                */

    /*
                            // Render output nodes first (order is important)
                            ImNodes::Ez::OutputSlots(node->output_slots.data(), (int)node->output_slots.size());

                            // Store new connections when they are created
                            Connection new_connection;
                            if (ImNodes::GetNewConnection(&new_connection.input_node, &new_connection.input_slot,
                                &new_connection.output_node, &new_connection.output_slot))
                            {
                                ((ViewNode*)new_connection.input_node)->connections.push_back(new_connection);
                                ((ViewNode*)new_connection.output_node)->connections.push_back(new_connection);
                            }

                            // Render output connections of this node
                            for (const Connection& connection : node->connections)
                            {
                                // Node contains all it's connections (both from output and to input slots). This means that multiple
                                // nodes will have same connection. We render only output connections and ensure that each connection
                                // will be rendered once.
                                if (connection.output_node != node)
                                    continue;

                                if (!ImNodes::Connection(connection.input_node, connection.input_slot, connection.output_node,
                                    connection.output_slot))
                                {
                                    // Remove deleted connections
                                    ((ViewNode*)connection.input_node)->DeleteConnection(connection);
                                    ((ViewNode*)connection.output_node)->DeleteConnection(connection);
                                }
                            }
                        }
                        // Node rendering is done. This call will render node background based on size of content inside node.
                        ImNodes::Ez::EndNode();

                        /*
                        if (node->selected && ImGui::IsKeyPressedMap(ImGuiKey_Delete))
                        {
                            for (auto& connection : node->connections)
                            {
                                ((ViewNode*)connection.input_node)->DeleteConnection(connection);
                                ((ViewNode*)connection.output_node)->DeleteConnection(connection);
                            }
                            delete node;
                            it = m_viewNodes.erase(it);
                        }
                        else
                        */
    //++it;
    /*}

                const ImGuiIO& io = ImGui::GetIO();
                if (ImGui::IsMouseReleased(1) && ImGui::IsWindowHovered() && !ImGui::IsMouseDragging(1))
                {
                    ImGui::FocusWindow(ImGui::GetCurrentWindow());
                    ImGui::OpenPopup("NodesContextMenu");
                }

                if (ImGui::BeginPopup("NodesContextMenu"))
                {
                    /*
                    for (const auto& desc : available_nodes)
                    {
                        if (ImGui::MenuItem(desc.first.c_str()))
                        {
                            nodes.push_back(desc.second());
                            ImNodes::AutoPositionNode(nodes.back());
                        }
                    }
                    ImGui::Separator();
                    if (ImGui::MenuItem("Reset Zoom"))
                        canvas.zoom = 1;

                    if (ImGui::IsAnyMouseDown() && !ImGui::IsWindowHovered())
                        ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                }

                ImNodes::EndCanvas();
            */
}

/*
std::map<std::string, MyNode*(*)()> available_nodes{
    {"Oscillator", []() -> MyNode* { return new MyNode("Oscillator", {
        {"Input", NodeSlotPosition},    {"Frequency", NodeSlotRotation}  // Input slots
    }, {
        {"Output", NodeSlotMatrix}                                      // Output slots
    }); }},
    {"ADSR", []() -> MyNode* { return new MyNode("ADSR", {
        {"Input", NodeSlotMatrix}                                      // Input slots
    }, {
        {"Output", NodeSlotPosition}  // Output slots
    }); }},
    {"Chorus", []() -> MyNode* { return new MyNode("Chorus", {
        {"Input", NodeSlotPosition}
    }, {
        {"Output", NodeSlotMatrix}                                      // Output slots
    }); }},
};
*/

} // namespace NodeGraph
