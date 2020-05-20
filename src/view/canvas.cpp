#include "nodegraph/view/canvas.h"
#include <imgui/imgui.h>

using namespace MUtils;

namespace NodeGraph
{

const NVec2f Canvas::PixelToView(const NVec2f& pixel) const
{
    return m_origin + (pixel / m_pixelRect.Size()) * m_scale;
}

const NVec2f Canvas::ViewToPixel(const NVec2f& view) const
{
    auto pixelSize = m_pixelRect.Size();
    return pixelSize * (view / m_viewport.Size());
}

void Canvas::Update()
{
    if (!ImGui::GetIO().WantCaptureKeyboard && !ImGui::GetIO().WantCaptureMouse)
    {
        auto mouse = ImGui::GetIO().MousePos;
        auto viewUnderMouse = PixelToView(NVec2f(mouse.x, mouse.y));

        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f)
        {
            wheel *= 20.0f;
            float ratio = m_pixelRect.Height() / m_pixelRect.Width();
            m_viewport.Adjust(wheel, wheel * ratio, -wheel, -wheel * ratio);
        
            auto newView = PixelToView(NVec2f(mouse.x, mouse.y));
            auto diff = newView - viewUnderMouse;
            m_viewport.Adjust(-diff.x, -diff.y);
        }
    }
}

void Canvas::SetPixelRect(const MUtils::NRectf& rc)
{
    m_pixelRect = rc;
    if (m_viewport.Empty())
    {
        m_viewport = m_pixelRect;
    }
}

NVec2f Canvas::WorldToView(const MUtils::NVec2f& pos) const
{
    NVec2f pixelsPerView = m_pixelRect.Size() / m_viewport.Size();
    NVec2f viewPos = (pos - m_viewport.topLeftPx) * pixelsPerView;
    return viewPos;
}

NRectf Canvas::WorldToView(const MUtils::NRectf& rc) const
{
    NVec2f pixelsPerView = m_pixelRect.Size() / m_viewport.Size();

    NVec2f viewPos1 = ((rc.topLeftPx - m_viewport.topLeftPx) * pixelsPerView);
    NVec2f viewPos2 = ((rc.bottomRightPx - m_viewport.topLeftPx) * pixelsPerView);

    return NRectf(viewPos1, viewPos2);
}

MUtils::NVec2f Canvas::WorldSizeToViewSize(const MUtils::NVec2f& size) const
{
    return ((size / m_viewport.Size()) * m_pixelRect.Size());
}

float Canvas::WorldSizeToViewSizeX(float size) const
{
    return ((size / m_viewport.Width()) * m_pixelRect.Width());
}

float Canvas::WorldSizeToViewSizeY(float size) const
{
    return ((size / m_viewport.Height()) * m_pixelRect.Height());
}

void CanvasVG::FilledCircle(const MUtils::NVec2f& center, float radius, const MUtils::NVec4f& color)
{
    auto viewCenter = WorldToView(center);
    auto viewRadius = WorldSizeToViewSizeX(radius);
    nvgBeginPath(vg);
    nvgFillColor(vg, ToNVGColor(color));
    nvgCircle(vg, viewCenter.x, viewCenter.y, viewRadius);
    nvgFill(vg);
}

void CanvasVG::FilledGradientCircle(const MUtils::NVec2f& center, float radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const NVec4f& endColor)
{
    auto viewCenter = WorldToView(center);
    auto viewRadius = WorldSizeToViewSizeX(radius);
    auto viewGradientBegin = WorldToView(gradientRange.topLeftPx);
    auto viewGradientEnd = WorldToView(gradientRange.bottomRightPx);
    nvgBeginPath(vg);
    nvgCircle(vg, viewCenter.x, viewCenter.y, viewRadius);
    auto bg = nvgLinearGradient(vg, viewGradientBegin.x, viewGradientBegin.y, viewGradientEnd.x, viewGradientEnd.y, ToNVGColor(startColor), ToNVGColor(endColor));
    nvgFillPaint(vg, bg);
    nvgFill(vg);
}

void CanvasVG::Stroke(const NVec2f& from, const NVec2f& to, float width, const NVec4f& color)
{
    auto viewFrom = WorldToView(from);
    auto viewTo = WorldToView(to);
    auto viewWidth = WorldSizeToViewSizeX(width);
    nvgBeginPath(vg);
    nvgMoveTo(vg, viewFrom.x, viewFrom.y);
    nvgLineTo(vg, viewTo.x, viewTo.y);
    nvgStrokeWidth(vg, viewWidth);
    nvgStrokeColor(vg, ToNVGColor(color));
    nvgStroke(vg);
}

void CanvasVG::FillRoundedRect(const NRectf& rc, float radius, const NVec4f& color)
{
    auto viewRect = WorldToView(rc);
    auto viewSize = WorldSizeToViewSizeX(radius);

    nvgBeginPath(vg);
    nvgRoundedRect(vg, viewRect.Left(), viewRect.Top(), viewRect.Width(), viewRect.Height(), viewSize);
    nvgFillColor(vg, ToNVGColor(color));
    nvgFill(vg);
}

void CanvasVG::FillGradientRoundedRect(const NRectf& rc, float radius, const NRectf& gradientRange, const NVec4f& startColor, const NVec4f& endColor)
{
    auto viewRect = WorldToView(rc);
    auto viewSize = WorldSizeToViewSizeX(radius);
    auto viewGradientBegin = WorldToView(gradientRange.topLeftPx);
    auto viewGradientEnd = WorldToView(gradientRange.bottomRightPx);

    nvgBeginPath(vg);
    nvgRoundedRect(vg, viewRect.Left(), viewRect.Top(), viewRect.Width(), viewRect.Height(), viewSize);
    auto bg = nvgLinearGradient(vg, viewGradientBegin.x, viewGradientBegin.y, viewGradientEnd.x, viewGradientEnd.y, ToNVGColor(startColor), ToNVGColor(endColor));
    nvgFillPaint(vg, bg);
    nvgFill(vg);
}

void CanvasVG::FillGradientRoundedRectVarying(const NRectf& rc, const NVec4f& radius, const NRectf& gradientRange, const NVec4f& startColor, const NVec4f& endColor)
{
    auto viewRect = WorldToView(rc);
    auto viewSize0 = WorldSizeToViewSizeX(radius.x);
    auto viewSize1 = WorldSizeToViewSizeX(radius.y);
    auto viewSize2 = WorldSizeToViewSizeX(radius.z);
    auto viewSize3 = WorldSizeToViewSizeX(radius.w);
    auto viewGradientBegin = WorldToView(gradientRange.topLeftPx);
    auto viewGradientEnd = WorldToView(gradientRange.bottomRightPx);

    nvgBeginPath(vg);
    nvgRoundedRectVarying(vg, viewRect.Left(), viewRect.Top(), viewRect.Width(), viewRect.Height(), viewSize0, viewSize1, viewSize2, viewSize3);
    auto bg = nvgLinearGradient(vg, viewGradientBegin.x, viewGradientBegin.y, viewGradientEnd.x, viewGradientEnd.y, ToNVGColor(startColor), ToNVGColor(endColor));
    nvgFillPaint(vg, bg);
    nvgFill(vg);
}

void CanvasVG::FillRect(const NRectf& rc, const NVec4f& color)
{
    auto viewRect = WorldToView(rc);

    nvgBeginPath(vg);
    nvgRect(vg, viewRect.Left(), viewRect.Top(), viewRect.Width(), viewRect.Height());
    nvgFillColor(vg, ToNVGColor(color));
    nvgFill(vg);
}

MUtils::NRectf CanvasVG::TextBounds(const MUtils::NVec2f& pos, float size, const char* pszText) const
{
    // Return everything in World space, since we scale every draw call
    auto viewPos = pos;
    float bounds[4];
    nvgTextAlign(vg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
    nvgFontSize(vg, size);
    nvgTextBounds(vg, viewPos.x, viewPos.y, pszText, nullptr, &bounds[0]);

    return NRectf(bounds[0], bounds[1], bounds[2] - bounds[0], bounds[3] - bounds[1]);
}

void CanvasVG::Text(const NVec2f& pos, float size, const NVec4f& color, const char* pszText, const char* pszFace, uint32_t align)
{
    auto viewSize = WorldSizeToViewSizeY(size);
    auto viewPos = WorldToView(pos);
    nvgTextAlign(vg, (align & Canvas::TEXT_ALIGN_MIDDLE ? NVG_ALIGN_MIDDLE : NVG_ALIGN_TOP) |
       (align & Canvas::TEXT_ALIGN_CENTER ? NVG_ALIGN_CENTER : NVG_ALIGN_LEFT));
    nvgFontSize(vg, viewSize);
    if (pszFace == nullptr)
    {
        nvgFontFace(vg, "sans");
    }
    else
    {
        nvgFontFace(vg, pszFace);
    }
    nvgFillColor(vg, ToNVGColor(color));
    nvgText(vg, viewPos.x, viewPos.y, pszText, nullptr);
}

void CanvasVG::Arc(const NVec2f& pos, float radius, float width, const NVec4f& color, float startAngle, float endAngle)
{
    auto viewRadius = WorldSizeToViewSizeX(radius);
    auto viewPos = WorldToView(pos);
    auto viewWidth = WorldSizeToViewSizeX(width);
    nvgBeginPath(vg);
    nvgStrokeColor(vg, ToNVGColor(color));
    nvgArc(vg, viewPos.x, viewPos.y, viewRadius, nvgDegToRad(startAngle), nvgDegToRad(endAngle), NVG_CW);
    nvgStrokeWidth(vg, viewWidth);
    nvgStroke(vg);
}

} // namespace NodeGraph
