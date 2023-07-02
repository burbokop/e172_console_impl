#pragma once

#include "surface.h"
#include <e172/graphics/abstractrenderer.h>

namespace e172 {
class Variant;
using VariantVector = std::vector<e172::Variant>;
} // namespace e172

namespace e172::impl::console {

class Renderer : public e172::AbstractRenderer
{
    Writer m_writer;

public:
    Renderer(std::ostream &output);

    // AbstractRenderer interface
protected:
    virtual bool update() override;

public:
    virtual size_t presentEffectCount() const override { return 0; }
    virtual std::string presentEffectName(size_t index) const override { return ""; }
    virtual void drawEffect(size_t index, const e172::VariantVector &args) override {}
    virtual void setDepth(int64_t depth) override {}
    virtual void fill(uint32_t color) override;
    virtual void drawPixel(const e172::Vector<double> &point, e172::Color color) override;
    virtual void drawLine(const e172::Vector<double> &point0,
                          const e172::Vector<double> &point1,
                          uint32_t color) override;

    virtual void drawRect(const e172::Vector<double> &point0,
                          const e172::Vector<double> &point1,
                          uint32_t color,
                          const e172::ShapeFormat &format) override;

    virtual void drawSquare(const e172::Vector<double> &point, int radius, uint32_t color) override;
    virtual void drawCircle(const e172::Vector<double> &point, int radius, uint32_t color) override;
    virtual void drawDiagonalGrid(const e172::Vector<double> &point0,
                                  const e172::Vector<double> &point1,
                                  int interval,
                                  uint32_t color) override
    {}

    virtual void drawImage(const e172::Image &image,
                           const e172::Vector<double> &position,
                           double angle,
                           double zoom) override;

    virtual e172::Vector<double> drawString(const std::string &string,
                                            const e172::Vector<double> &position,
                                            uint32_t color,
                                            const e172::TextFormat &format) override
    {
        return {};
    }

    virtual void modifyBitmap(const std::function<void(e172::Color *bitmap)> &modifier) override;

    virtual void applyLensEffect(const e172::Vector<double> &point0,
                                 const e172::Vector<double> &point1,
                                 double coefficient) override
    {}

    virtual void applySmooth(const e172::Vector<double> &point0,
                             const e172::Vector<double> &point1,
                             double coefficient) override
    {}

    virtual void enableEffect(uint64_t effect) override {}
    virtual void disableEffect(uint64_t effect) override {}
    virtual void setFullscreen(bool value) override;
    virtual void setResolution(e172::Vector<double> value) override;
    virtual e172::Vector<double> resolution() const override;
    virtual e172::Vector<double> screenSize() const override;
};

} // namespace e172::impl::console
