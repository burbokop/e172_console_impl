#include "painter.h"

painter::painter() {}

void painter::paint(pixel_primitives::bitmap &btmp) {
    const auto fff = [](double radius, double t) -> double {
        return radius + t * 0.001;
    };

    double aspect = static_cast<double>(btmp.width) / btmp.height;

    for(std::size_t y = 0; y < btmp.height; ++y) {
        for(std::size_t x = 0; x < btmp.width; ++x) {
            const auto xx =  (static_cast<double>(x) / btmp.width * 2. - 1.) * aspect;
            const auto yy =  (static_cast<double>(y) / btmp.height * 2. - 1.);

            const auto t = m_timer.elapsed();
            const auto sin = std::sin(t * 0.001);

            const auto radius = xx * xx + yy * yy;
            const auto intensity = fff(radius * 2, t);
            const auto color = std::uint32_t(intensity * 0xffffff) % 0xffffff;

            auto& pix = pixel_primitives::pixel(btmp, x, y);

            pix = radius < 0.5 ? color : 0x000000;
            pix |= (std::uint8_t(intensity * 0xff) % 0xff) << 24;
        }
    }
}
