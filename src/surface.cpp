#include "surface.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <ostream>

bool surface::auto_resize() const {
    return m_auto_resize;
}

void surface::set_auto_resize(bool newAuto_resize) {
    m_auto_resize = newAuto_resize;
}

surface::surface(std::ostream &output, const colorizer *colorizer, const std::string &gradient, double contrast, bool auto_resize, uint32_t mask, bool ignore_alpha, double symbol_wh_fraction)
    : m_output(output),
      m_colorizer(colorizer),
      m_gradient(gradient),
      m_contrast(contrast),
      m_auto_resize(auto_resize),
      m_mask(mask),
      m_ignore_alpha(ignore_alpha),
      m_symbol_wh_fraction(symbol_wh_fraction) {}


char surface::charFromArgb(uint32_t argb) const {
    return charFromBrightness((std::uint8_t(argb) + std::uint8_t(argb >> 8) + std::uint8_t(argb >> 16)) * std::uint8_t(argb >> 24) / 3 / 0xff);
}

char surface::charFromBrightness(uint8_t brightness) const {
    if(std::abs(m_contrast - 1) >= std::numeric_limits<double>::epsilon()) {
        brightness = (brightness - 0x88) * m_contrast + 0x88;
    }
    return m_gradient[brightness * (m_gradient.size() - 1) / 0xff];
}

std::pair<std::size_t, std::size_t> surface::console_size(int fd) const {
    int cols = 80;
    int lines = 24;

#ifdef TIOCGSIZE
    struct ttysize ts;
    ioctl(fd, TIOCGSIZE, &ts);
    cols = ts.ts_cols;
    lines = ts.ts_lines;
#elif defined(TIOCGWINSZ)
    struct winsize ts;
    ioctl(fd, TIOCGWINSZ, &ts);
    cols = ts.ws_col;
    lines = ts.ws_row;
#endif /* TIOCGSIZE */

    return { cols * m_symbol_wh_fraction, lines - 1 /* one line is input */ };
}

void surface::set_size(std::size_t w, std::size_t h) {
    if(w != m_bitmap.width || h != m_bitmap.height) {
        m_bitmap.width = w;
        m_bitmap.height = h;
        if(w != 0 && h != 0) {
            if(m_bitmap) {
                delete m_bitmap.matrix;
            }
            m_bitmap.matrix = new uint32_t[w * h];
        }
    }
}

std::size_t surface::update() {
    std::size_t result = 0;
    if(m_bitmap && m_bitmap.width > 0 && m_bitmap.height > 0) {
        const auto w = m_bitmap.width / m_symbol_wh_fraction;
        const auto h = m_bitmap.height;

        std::string buffer; { buffer.reserve(); }
        std::string lastColorCode;
        for(std::size_t y = 0; y < h; ++y) {
            for(std::size_t x = 0; x < w; ++x) {

                std::uint32_t argb = pixel_primitives::pixel(m_bitmap, x * m_symbol_wh_fraction, y);


                //std::uint32_t argb = m_bitmap[y * m_bitmap.width + x];
                if(m_ignore_alpha) { argb |= 0xff000000; }
                argb &= m_mask;

                if(m_colorizer) {
                    std::string cc = m_colorizer->beginSeq(argb);
                    if(cc != lastColorCode) {
                        if(cc.size() > 0) {
                            buffer += cc;
                        } else {
                            buffer += m_colorizer->endSeq();
                        }
                        lastColorCode = cc;
                    }
                }
                buffer += charFromArgb(argb);
            }
            buffer += '\n';
        }
        m_output.write(buffer.c_str(), buffer.size());
        result = buffer.size();
    }
    if(m_auto_resize) {
        const auto& size = console_size(STDOUT_FILENO);
        set_size(size.first, size.second);
    }
    return result;
}

surface::~surface() {
    if(m_bitmap) {
        delete m_bitmap.matrix;
    }
}
