#ifndef SURFACE_H
#define SURFACE_H

#include <vector>
#include "pixelprimitives.h"
#include "colorizer/colorizer.h"

class surface {
    pixel_primitives::bitmap m_bitmap;
    std::ostream& m_output;
    const colorizer *m_colorizer;
    const std::string m_gradient;
    double m_contrast;
    bool m_auto_resize;
    std::uint32_t m_mask;
    bool m_ignore_alpha;
    double m_symbol_wh_fraction;
public:
    surface(
            std::ostream& output,
            const colorizer *colorizer = nullptr,
            const std::string &gradient = default_gradient,
            double contrast = 1,
            bool auto_resize = true,
            std::uint32_t mask = 0xffffffff,
            bool ignore_alpha = false,
            double symbol_wh_fraction = 11. / 24.
            );

    static constexpr const char default_gradient[] = " .:!/r(l1Z4H9W8$@";
    char charFromArgb(std::uint32_t argb) const;
    char charFromBrightness(std::uint8_t brightness) const;

    std::pair<std::size_t, std::size_t> console_size(int fd) const;
    void set_size(std::size_t w, std::size_t h);
    std::size_t update();

    pixel_primitives::bitmap& bitmap() { return m_bitmap; }
    const pixel_primitives::bitmap& bitmap() const { return m_bitmap; }

    ~surface();
    bool auto_resize() const;
    void set_auto_resize(bool newAuto_resize);
};

#endif // SURFACE_H
