#include "surface.h"
#include <sys/ioctl.h>
#include <iostream>
#include "color.h"
#include <unistd.h>


bool surface::auto_resize() const {
    return m_auto_resize;
}

void surface::set_auto_resize(bool newAuto_resize) {
    m_auto_resize = newAuto_resize;
}

surface::surface(colorizer *colorizer, bool auto_resize, uint32_t mask, bool ignore_alpha, double symbol_wh_fraction)
    : m_colorizer(colorizer),
      m_auto_resize(auto_resize),
      m_mask(mask),
      m_ignore_alpha(ignore_alpha),
      m_symbol_wh_fraction(symbol_wh_fraction) {}

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

void surface::update() {
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
                buffer += character::argbToChar(argb);
            }
            buffer += '\n';
        }
        std::cout.write(buffer.c_str(), buffer.size());
    }
    if(m_auto_resize) {
        const auto& size = console_size(STDOUT_FILENO);
        set_size(size.first, size.second);
    }
}

surface::~surface() {
    if(m_bitmap) {
        delete m_bitmap.matrix;
    }
}
