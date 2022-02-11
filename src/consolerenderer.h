#ifndef CONSOLERENDERER_H
#define CONSOLERENDERER_H

#include "surface.h"

#include <e172/graphics/abstractrenderer.h>

namespace e172 {
    class Variant;
    typedef std::vector<e172::Variant> VariantVector;
}

class ConsoleRenderer : public e172::AbstractRenderer {
    console_writer m_writer;
public:
    ConsoleRenderer(std::ostream &output);

    // AbstractRenderer interface
protected:
    bool update();

public:
    size_t presentEffectCount() const { return 0; }
    std::string presentEffectName(size_t index) const { return ""; }
    void drawEffect(size_t index, const e172::VariantVector &args) {}
    void setDepth(int64_t depth);
    void fill(uint32_t color);
    void drawPixel(const e172::Vector &point, uint32_t color);
    void drawLine(const e172::Vector &point0, const e172::Vector &point1, uint32_t color);
    void drawRect(const e172::Vector &point0, const e172::Vector &point1, uint32_t color, const e172::ShapeFormat &format);
    void drawSquare(const e172::Vector &point, int radius, uint32_t color);
    void drawCircle(const e172::Vector &point, int radius, uint32_t color);
    void drawDiagonalGrid(const e172::Vector &point0, const e172::Vector &point1, int interval, uint32_t color) {}
    void drawImage(const e172::Image &image, const e172::Vector &position, double angle, double zoom) {}
    e172::Vector drawString(const std::string &string, const e172::Vector &position, uint32_t color, const e172::TextFormat &format) { return {}; }
    e172::Color *bitmap() const { return nullptr; }
    void applyLensEffect(const e172::Vector &point0, const e172::Vector &point1, double coefficient) {}
    void applySmooth(const e172::Vector &point0, const e172::Vector &point1, double coefficient) {}
    void enableEffect(uint64_t effect) {}
    void disableEffect(uint64_t effect) {}
    void setFullscreen(bool value) {}
    void setResolution(e172::Vector value);
    e172::Vector resolution() const;
    e172::Vector screenSize() const;
};

#endif // CONSOLERENDERER_H
