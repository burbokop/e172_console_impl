#include "surface.h"

#include <cerrno>
#include <cstdio>
#include <e172/consolecolor.h>
#include <ext/stdio_filebuf.h>
#include <ext/stdio_sync_filebuf.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sys/ioctl.h>
#include <unistd.h>

namespace e172::impl::console {

namespace {

constexpr const char *ClearScreenSeq = "\x1B[2J\x1B[H";

}

std::ostream &Writer::output() const
{
    return m_output;
}

Writer::Writer(std::ostream &output, const Style &style)
    : m_output(output)
    , m_style(style)
{}

char Writer::charFromArgb(uint32_t argb) const
{
    return charFromBrightness((std::uint8_t(argb) + std::uint8_t(argb >> 8) + std::uint8_t(argb >> 16)) * std::uint8_t(argb >> 24) / 3 / 0xff);
}

char Writer::charFromBrightness(uint8_t brightness) const
{
    if (std::abs(m_style.contrast - 1) >= std::numeric_limits<double>::epsilon()) {
        brightness = (brightness - 0x88) * m_style.contrast + 0x88;
    }
    return m_style.gradient[brightness * (m_style.gradient.size() - 1) / 0xff];
}

e172::Vector<uint32_t> Writer::outputStreamSize(const std::ostream &stream, double whFraction)
{
    if (const auto &fd = outputStreamDescriptor(stream)) {
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

        //std::cout << "fd: " << *fd << ", cols: " << cols << ", lines: " << lines << std::endl;

        assert(cols >= 0);
        assert(lines >= 0);

        return {static_cast<std::uint32_t>(cols * whFraction),
                static_cast<std::uint32_t>(lines - 1) /* one line is input */};
    }
    return { 0, 0 };
}

std::optional<int> Writer::outputStreamDescriptor(const std::ostream &stream)
{
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

void Writer::setFrameSize(std::size_t w, std::size_t h)
{
    if(w != m_bitmap.width || h != m_bitmap.height) {
        if(w != 0 && h != 0) {
            if(m_bitmap) {
                delete m_bitmap.matrix;
            }
            m_bitmap = pixel_primitives::bitmap { new uint32_t[w * h], w, h };
        }
    }
}

std::size_t Writer::writeFrame()
{
    std::size_t result = 0;
    if(m_bitmap && m_bitmap.width > 0 && m_bitmap.height > 0) {
        const auto w = m_bitmap.width / m_style.symbolWHFraction;
        const auto h = m_bitmap.height;

        std::string buffer; //{ buffer.reserve(); }
        std::string lastColorCode;
        for(std::size_t y = 0; y < h; ++y) {
            for(std::size_t x = 0; x < w; ++x) {
                std::uint32_t argb = pixel_primitives::pixel(m_bitmap,
                                                             x * m_style.symbolWHFraction,
                                                             y);

                if (m_style.ignoreAlpha) {
                    argb |= 0xff000000;
                }
                argb &= m_style.mask;

                if (m_style.colorizer) {
                    std::string cc = m_style.colorizer->beginSeq(argb);
                    if(cc != lastColorCode) {
                        if(cc.size() > 0) {
                            buffer += cc;
                        } else {
                            buffer += m_style.colorizer->endSeq();
                        }
                        lastColorCode = cc;
                    }
                }
                buffer += charFromArgb(argb);
            }
            buffer += '\n';
        }
        m_output.write(buffer.c_str(), buffer.size());
        m_output << e172::cc::Default;
        result = buffer.size();
    }
    if (m_autoResize) {
        const auto &size = outputStreamSize(m_output, m_style.symbolWHFraction);
        setFrameSize(size.x(), size.y());
    }
    return result;
}

Writer::~Writer()
{
    if(m_bitmap) {
        delete m_bitmap.matrix;
    }
    m_output << ClearScreenSeq;
}

} // namespace e172::impl::console
