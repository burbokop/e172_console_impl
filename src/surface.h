#pragma once

#include "colorizer/colorizer.h"
#include "pixelprimitives.h"
#include <e172/math/vector.h>
#include <memory>
#include <optional>
#include <vector>

namespace e172::impl::console {

static constexpr const char DefaultGradient[] = " .:!/r(l1Z4H9W8$@";

struct Style
{
    std::shared_ptr<const Colorizer> colorizer = nullptr;
    std::string gradient = DefaultGradient;
    double contrast = 1;
    std::uint32_t mask = 0xffffffff;
    bool ignoreAlpha = false;
    double symbolWHFraction = 11. / 24.;
};

class Writer
{
public:
    Writer(std::ostream &output, const Style &style);

    char charFromArgb(std::uint32_t argb) const;
    char charFromBrightness(std::uint8_t brightness) const;

    static std::optional<int> outputStreamDescriptor(const std::ostream &stream);
    static e172::Vector<std::uint32_t> outputStreamSize(const std::ostream &stream,
                                                        double whFraction = 1);

    void setFrameSize(std::size_t w, std::size_t h);
    std::size_t writeFrame();

    pixel_primitives::bitmap& bitmap() { return m_bitmap; }
    const pixel_primitives::bitmap& bitmap() const { return m_bitmap; }

    ~Writer();

    bool autoResize() const { return m_autoResize; }
    void setAutoResize(bool v) { m_autoResize = v; }

    std::ostream &output() const;
    const Style &style() const { return m_style; }

private:
    pixel_primitives::bitmap m_bitmap;
    std::ostream &m_output;
    Style m_style;
    bool m_autoResize = true;
};

} // namespace e172::impl::console
