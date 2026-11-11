// Copyright 2023 Borys Boiko

#pragma once

#include "surface.h"
#include <e172/graphics/abstractrenderer.h>
#include <string>
#include <vector>

namespace e172 {
class Variant;
using VariantVector = std::vector<e172::Variant>;
} // namespace e172

namespace e172::impl::console {

class GraphicsProvider;

class Renderer : public e172::AbstractRenderer {
    friend GraphicsProvider;
    struct Private {};

public:
    Renderer(
        Private,
        std::ostream& output,
        const Style& style);

    // AbstractRenderer interface
protected:
    bool update() override;

    // AbstractRenderer interface
public:
    size_t presentEffectCount() const override { return 0; }
    std::string presentEffectName(std::size_t) const override { return ""; }
    void drawEffect(
        std::size_t,
        const e172::VariantVector&) override
    {
    }
    void setDepth(std::int64_t) override {}
    void fill(Color color) override;
    void drawPixel(
        const e172::Vector<double>& point,
        e172::Color color) override;
    void drawLine(
        const e172::Vector<double>& point0,
        const e172::Vector<double>& point1,
        Color color) override;

    void drawRect(
        const e172::Vector<double>& point0,
        const e172::Vector<double>& point1,
        Color color,
        const e172::ShapeFormat& format) override;

    void drawSquare(
        const e172::Vector<double>& center,
        double radius,
        Color color) override;
    void drawCircle(
        const e172::Vector<double>& center,
        double radius,
        Color color) override;
    void drawDiagonalGrid(
        const e172::Vector<double>&,
        const e172::Vector<double>&,
        double,
        std::uint32_t) override
    {
    }

    void drawImage(
        const e172::Image& image,
        const e172::Vector<double>& center,
        double angle,
        double zoom) override;

    e172::Vector<double> drawString(
        const std::string&,
        const e172::Vector<double>&,
        std::uint32_t,
        const e172::TextFormat&) override
    {
        return {};
    }

    void modifyBitmap(const std::function<void(e172::Color* bitmap)>& modifier) override;

    void applyLensEffect(
        const e172::Vector<double>&,
        const e172::Vector<double>&,
        double) override
    {
    }

    void applySmooth(
        const e172::Vector<double>&,
        const e172::Vector<double>&,
        double) override
    {
    }

    void enableEffect(std::uint64_t) override {}
    void disableEffect(std::uint64_t) override {}
    void setFullscreen(bool value) override;
    void setResolution(const e172::Vector<std::uint32_t>& value) override;
    e172::Vector<std::uint32_t> resolution() const override;

private:
    Writer m_writer;
    Vector<double> m_position;
};

} // namespace e172::impl::console
