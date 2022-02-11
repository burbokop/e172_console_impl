#include "surface.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <ostream>

#include <cstdio>
#include <fstream>
#include <cerrno>
#include <iostream>
#include <ext/stdio_filebuf.h>
#include <ext/stdio_sync_filebuf.h>

bool console_writer::auto_resize() const {
    return m_auto_resize;
}

void console_writer::set_auto_resize(bool newAuto_resize) {
    m_auto_resize = newAuto_resize;
}

std::ostream &console_writer::output() const {
    return m_output;
}

double console_writer::symbol_wh_fraction() const {
    return m_symbol_wh_fraction;
}

console_writer::console_writer(std::ostream &output, const colorizer *colorizer, const std::string &gradient, double contrast, bool auto_resize, uint32_t mask, bool ignore_alpha, double symbol_wh_fraction)
    : m_output(output),
      m_colorizer(colorizer),
      m_gradient(gradient),
      m_contrast(contrast),
      m_auto_resize(auto_resize),
      m_mask(mask),
      m_ignore_alpha(ignore_alpha),
      m_symbol_wh_fraction(symbol_wh_fraction) {}


char console_writer::charFromArgb(uint32_t argb) const {
    return charFromBrightness((std::uint8_t(argb) + std::uint8_t(argb >> 8) + std::uint8_t(argb >> 16)) * std::uint8_t(argb >> 24) / 3 / 0xff);
}

char console_writer::charFromBrightness(uint8_t brightness) const {
    if(std::abs(m_contrast - 1) >= std::numeric_limits<double>::epsilon()) {
        brightness = (brightness - 0x88) * m_contrast + 0x88;
    }
    return m_gradient[brightness * (m_gradient.size() - 1) / 0xff];
}

std::pair<std::size_t, std::size_t> console_writer::output_stream_size(const std::ostream &stream, double wh_fraction) {
    if(const auto& fd = output_stream_descriptor(stream)) {
        int cols = 80;
        int lines = 24;

#ifdef TIOCGSIZE
        struct ttysize ts;
        ioctl(fd, TIOCGSIZE, &ts);
        cols = ts.ts_cols;
        lines = ts.ts_lines;
#elif defined(TIOCGWINSZ)
        struct winsize ts;
        ioctl(*fd, TIOCGWINSZ, &ts);
        cols = ts.ws_col;
        lines = ts.ws_row;
#endif /* TIOCGSIZE */


        std::cout << "fd: " << *fd << ", cols: " << cols << ", lines: " << lines << std::endl;

        return { cols * wh_fraction, lines - 1 /* one line is input */ };
    }
    return { 0, 0 };
}

std::optional<int> console_writer::output_stream_descriptor(const std::ostream &stream) {
    const auto& stdio_buf = dynamic_cast<__gnu_cxx::stdio_filebuf<char>*>(stream.rdbuf());
    if(stdio_buf) {
        return stdio_buf->fd();
    }
    const auto& stdiosync_buf = dynamic_cast<__gnu_cxx::stdio_sync_filebuf<char>*>(stream.rdbuf());
    if(stdiosync_buf) {
        return stdiosync_buf->file()->_fileno;
    }
    const auto& file_buf = dynamic_cast<std::filebuf*>(stream.rdbuf());
    if(file_buf) {
        struct tmp_filebuf : public std::filebuf {
            int fd() { return this->_M_file.fd(); }
        };
        return static_cast<tmp_filebuf*>(file_buf)->fd();
    }
    return std::nullopt;
}

void console_writer::set_frame_size(std::size_t w, std::size_t h) {
    if(w != m_bitmap.width || h != m_bitmap.height) {
        if(w != 0 && h != 0) {
            if(m_bitmap) {
                delete m_bitmap.matrix;
            }
            m_bitmap = pixel_primitives::bitmap { new uint32_t[w * h], w, h };
        }
    }
}

std::size_t console_writer::write_frame() {
    std::size_t result = 0;
    if(m_bitmap && m_bitmap.width > 0 && m_bitmap.height > 0) {
        const auto w = m_bitmap.width / m_symbol_wh_fraction;
        const auto h = m_bitmap.height;

        std::string buffer; { buffer.reserve(); }
        std::string lastColorCode;
        for(std::size_t y = 0; y < h; ++y) {
            for(std::size_t x = 0; x < w; ++x) {
                std::uint32_t argb = pixel_primitives::pixel(m_bitmap, x * m_symbol_wh_fraction, y);

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
        const auto& size = output_stream_size(m_output, m_symbol_wh_fraction);
        set_frame_size(size.first, size.second);
    }
    return result;
}

console_writer::~console_writer() {
    if(m_bitmap) {
        delete m_bitmap.matrix;
    }
}
