#pragma once

#include "../../src/pixelprimitives.h"
#include <cstdint>
#include <e172/graphics/image.h>
#include <e172/math/rational.h>
#include <filesystem>
#include <ostream>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif
#include <libavutil/pixfmt.h>

#ifdef __cplusplus
}
#endif

struct AVFrame;
struct AVPacket;
struct AVCodecContext;
struct SwsContext;
struct AVFormatContext;

namespace e172 {
class AbstractGraphicsProvider;
}

namespace e172::impl::console::video_player {

class MP4Decoder
{
public:
    struct Frame
    {
        pixel_primitives::bitmap bitmap;
        e172::Image image;
    };

    MP4Decoder(const std::filesystem::path &path, std::ostream &log, double scale);

    Frame &frame(std::size_t index, const e172::AbstractGraphicsProvider &graphicsProvider) const;

    std::size_t frameCount() const;

    Rational<std::uint32_t> frameRate() const;

    ~MP4Decoder();

    e172::Vector<std::uint32_t> size() const
    {
        return {static_cast<std::uint32_t>(m_destinationWidth),
                static_cast<std::uint32_t>(m_destinationHeight)};
    }

private:
    std::filesystem::path m_path;
    std::ostream &m_log;
    double m_scale;

    AVFormatContext *m_formatContext = nullptr;
    AVPacket *m_packet = nullptr;
    AVFrame *m_frame = nullptr;
    AVCodecContext *m_codecContext = nullptr;
    SwsContext *m_imageConvertContext = nullptr;
    int m_video_stream_index;
    mutable std::vector<Frame> m_cache;
    std::size_t m_destinationWidth;
    std::size_t m_destinationHeight;
    AVPixelFormat m_destinationFormat;
};

} // namespace e172::impl::console::video_player
