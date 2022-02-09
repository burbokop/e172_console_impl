#ifndef MP4_DECODER_H
#define MP4_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#ifdef __cplusplus
}
#endif

#include <ostream>
#include <vector>
#include "pixelprimitives.h"
#include <cstdint>

class mp4_decoder {
public:
    mp4_decoder();

    static std::uint8_t canal(AVFrame* frame, std::size_t canal_no, std::size_t x, std::size_t y, std::size_t width);

    static int decode(std::vector<pixel_primitives::bitmap>& dst_btmps, const char *path, std::ostream &log, std::size_t packet_count_to_process, double scale);
    static int decode_packet(std::vector<pixel_primitives::bitmap>& dst_btmps, AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame, std::ostream &log, double scale);
    static void save_gray_frame(uint8_t *buf, int wrap, int xsize, int ysize, char *filename, std::ostream &log);
};

#endif // MP4_DECODER_H
