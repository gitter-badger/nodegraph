#pragma once

#include <mutils/math/math.h>

#include <nanovg/nanovg.h>

#include "nodegraph/model/graph.h"

namespace NodeGraph
{

class Canvas
{
public:
    Canvas(Graph& graph)
        : m_graph(graph)
    {}

    const MUtils::NVec2f PixelToView(const MUtils::NVec2f& pixel) const; 

    virtual MUtils::NVec2f ViewToPixels(const MUtils::NVec2f& pos) const;
    virtual MUtils::NRectf ViewToPixels(const MUtils::NRectf& rc) const ;
    virtual float WorldSizeToViewSizeX(float size) const;
    virtual float WorldSizeToViewSizeY(float size) const;
    virtual MUtils::NVec2f ViewSizeToPixelSize(const MUtils::NVec2f& size) const;

    void SetPixelRect(const MUtils::NRectf& rc);
    MUtils::NRectf GetPixelRect() const { return m_pixelRect; }

    virtual void FilledCircle(const MUtils::NVec2f& center, float radius, const MUtils::NVec4f& color) = 0;
    virtual void FilledGradientCircle(const MUtils::NVec2f& center, float radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const MUtils::NVec4f& endColor) = 0;
    virtual void FillRoundedRect(const MUtils::NRectf& rc, float radius, const MUtils::NVec4f& color) = 0;
    virtual void FillRect(const MUtils::NRectf& rc, const MUtils::NVec4f& color) = 0;
    virtual void FillGradientRoundedRect(const MUtils::NRectf& rc, float radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const MUtils::NVec4f& endColor) = 0;
    virtual void FillGradientRoundedRectVarying(const MUtils::NRectf& rc, const MUtils::NVec4f& radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const MUtils::NVec4f& endColor) = 0;

    virtual void Stroke(const MUtils::NVec2f& from, const MUtils::NVec2f& to, float width, const MUtils::NVec4f& color) = 0;

    virtual void Arc(const MUtils::NVec2f& pos, float radius, float width, const MUtils::NVec4f& color, float startAngle, float endAngle) = 0;

    virtual MUtils::NRectf TextBounds(const MUtils::NVec2f& pos, float size, const char* pszText) const = 0;

    enum TextAlign
    {
        TEXT_ALIGN_MIDDLE = 1,
        TEXT_ALIGN_CENTER = 2,
        TEXT_ALIGN_TOP = 4,
        TEXT_ALIGN_LEFT = 8,
    };

    virtual void Text(const MUtils::NVec2f& pos, float size, const MUtils::NVec4f& color, const char* pszText, const char* pszFace = nullptr, uint32_t align = TEXT_ALIGN_MIDDLE | TEXT_ALIGN_CENTER) = 0;
    virtual void Update(const MUtils::NVec2f& size, const MUtils::NVec2f& mousePos);

    virtual MUtils::NVec2f GetMousePos() const { return m_mousePos;  }
    virtual MUtils::NVec2f GetViewMousePos() const { return PixelToView(m_mousePos); }
private:
    Graph& m_graph;
    MUtils::NRectf m_pixelRect; // Pixel size on screen of canvas
    MUtils::NVec2f m_mousePos;

    MUtils::NVec2f m_viewOrigin;
    float m_viewScale = 1.0f;
};

class CanvasVG : public Canvas
{
public:
    CanvasVG(Graph& graph, NVGcontext* vgContext)
        : Canvas(graph),
        vg(vgContext)
    {

    }

    NVGcontext* GetVG() const { return vg; }
    NVGcolor ToNVGColor(const MUtils::NVec4f& val) { return nvgRGBAf(val.x, val.y, val.z, val.w); }
    virtual void FilledCircle(const MUtils::NVec2f& center, float radius, const MUtils::NVec4f& color) override;
    virtual void FilledGradientCircle(const MUtils::NVec2f& center, float radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const MUtils::NVec4f& endColor) override;
    virtual void FillRoundedRect(const MUtils::NRectf& rc, float radius, const MUtils::NVec4f& color) override;
    virtual void FillGradientRoundedRect(const MUtils::NRectf& rc, float radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const MUtils::NVec4f& endColor) override;
    virtual void FillGradientRoundedRectVarying(const MUtils::NRectf& rc, const MUtils::NVec4f& radius, const MUtils::NRectf& gradientRange, const MUtils::NVec4f& startColor, const MUtils::NVec4f& endColor) override;
    virtual void FillRect(const MUtils::NRectf& rc, const MUtils::NVec4f& color) override;

    virtual void Text(const MUtils::NVec2f& pos, float size, const MUtils::NVec4f& color, const char* pszText, const char* pszFace = nullptr, uint32_t align = TEXT_ALIGN_MIDDLE | TEXT_ALIGN_CENTER) override;
    virtual MUtils::NRectf TextBounds(const MUtils::NVec2f& pos, float size, const char* pszText) const override;

    virtual void Stroke(const MUtils::NVec2f& from, const MUtils::NVec2f& to, float width, const MUtils::NVec4f& color) override;
    
    virtual void Arc(const MUtils::NVec2f& pos, float radius, float width, const MUtils::NVec4f& color, float startAngle, float endAngle) override;
private:
    NVGcontext* vg = nullptr;
};

} // namespace NodeGraph