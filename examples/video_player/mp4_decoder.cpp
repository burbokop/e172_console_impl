#include "mp4_decoder.h"

#include <e172/utility/either.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
#ifdef __cplusplus
}
#endif

#include <e172/graphics/abstractgraphicsprovider.h>

namespace e172::impl::console::video_player {

namespace {

void copyFrameToBuffer(Byte *dst,
                       AVFrame *frame,
                       AVCodecContext *codecContext,
                       SwsContext *imageConvertContext,
                       std::size_t destinationWidth,
                       std::size_t destinationHeight,
                       AVPixelFormat destinationFormat)
{
    constexpr std::size_t align = 1;
    struct Picture
    {
        std::uint8_t *data[AV_NUM_DATA_POINTERS];
        int linesize[AV_NUM_DATA_POINTERS];
    };

    Picture picture;
    av_image_fill_arrays(picture.data,
                         picture.linesize,
                         dst,
                         destinationFormat,
                         destinationWidth,
                         destinationHeight,
                         align);

    sws_scale(imageConvertContext,
              frame->data,
              frame->linesize,
              0,
              codecContext->height,
              picture.data,
              picture.linesize);
}

struct DecodeError
{
    std::string during;
    int averrcode;
};

std::ostream &operator<<(std::ostream &stream, DecodeError err)
{
    return stream << "Decode error " << err.during << ": " << av_err2str(err.averrcode);
}

Either<DecodeError, Void> decodePacket(std::vector<MP4Decoder::Frame> &dst_btmps,
                                       AVPacket *packet,
                                       AVCodecContext *codecContext,
                                       SwsContext *imageConvertContext,
                                       AVFrame *frame,
                                       std::ostream &log,
                                       std::size_t destinationWidth,
                                       std::size_t destinationHeight,
                                       AVPixelFormat destinationFormat,
                                       const e172::AbstractGraphicsProvider &graphicsProvider)
{
    auto response = avcodec_send_packet(codecContext, packet);
    if (response < 0) {
        return Left(
            DecodeError{.during = "during sending a packet to the decoder", .averrcode = response});
    }

    while (response >= 0) {
        response = avcodec_receive_frame(codecContext, frame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
            break;
        } else if (response < 0) {
            return Left(DecodeError{.during = "during receiving a frame from the decoder",
                                    .averrcode = response});
        }

        if (response >= 0) {
            bool log_frame = false;

            if (log_frame) {
                log << "Frame " << codecContext->frame_number
                    << " (type=" << av_get_picture_type_char(frame->pict_type)
                    << ", size=" << frame->pkt_size
                    << " bytes, format=" << av_get_sample_fmt_name(AVSampleFormat(frame->format))
                    << ") pts " << frame->pts << " key_frame " << frame->key_frame << " [DTS "
                    << frame->coded_picture_number << "]" << std::endl;
            }

            if (log_frame) {
                log << "\tw: " << frame->width << ", h: " << frame->height << std::endl;
                log << "\tlen 0: " << frame->linesize[0] << std::endl;
                log << "\tlen 1: " << frame->linesize[1] << std::endl;
                log << "\tlen 2: " << frame->linesize[2] << std::endl;
                log << "\tlen 3: " << frame->linesize[3] << std::endl;
                log << "\tlen 4: " << frame->linesize[4] << std::endl;
                log << "\tlen 5: " << frame->linesize[5] << std::endl;
                log << "\tlen 6: " << frame->linesize[6] << std::endl;
                log << "\tlen 7: " << frame->linesize[7] << std::endl;
            }

            pixel_primitives::bitmap btmp{new std::uint32_t[destinationWidth * destinationHeight],
                                          static_cast<size_t>(destinationWidth),
                                          static_cast<size_t>(destinationHeight)};

            /* TODO: remove */ {
                copyFrameToBuffer(reinterpret_cast<std::uint8_t *>(btmp.matrix),
                                  frame,
                                  codecContext,
                                  imageConvertContext,
                                  destinationWidth,
                                  destinationHeight,
                                  destinationFormat);
            }

            const auto image = graphicsProvider.createImage(
                destinationWidth,
                destinationHeight,
                [&destinationFormat,
                 &imageConvertContext,
                 &frame,
                 &codecContext](std::size_t w, std::size_t h, e172::Color *pixels) {
                    copyFrameToBuffer(reinterpret_cast<std::uint8_t *>(pixels),
                                      frame,
                                      codecContext,
                                      imageConvertContext,
                                      w,
                                      h,
                                      destinationFormat);
                });

            dst_btmps.push_back({btmp, image});
        }
    }
    return Right(Void{});
}

} // namespace

MP4Decoder::MP4Decoder(const std::filesystem::path &path, std::ostream &log, double scale)
    : m_path(path)
    , m_log(log)
    , m_scale(scale)
{
    if (path.empty()) {
        log << "You need to specify a media file." << std::endl;
        return;
    }

    log << "initializing all the containers, codecs and protocols." << std::endl;

    // AVFormatContext holds the header information from the format (Container)
    // Allocating memory for this component
    // http://ffmpeg.org/doxygen/trunk/structAVFormatContext.html
    if (!(m_formatContext = avformat_alloc_context())) {
        log << "ERROR could not allocate memory for Format Context" << std::endl;
        return;
    }

    log << "opening the input file " << path << " and loading format (container) header" << std::endl;
    // Open the file and read its header. The codecs are not opened.
    // The function arguments are:
    // AVFormatContext (the component we allocated memory for),
    // url (filename),
    // AVInputFormat (if you pass NULL it'll do the auto detect)
    // and AVDictionary (which are options to the demuxer)
    // http://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#ga31d601155e9035d5b0e7efedc894ee49
    if (avformat_open_input(&m_formatContext, path.c_str(), NULL, NULL) != 0) {
        log << "ERROR could not open the file" << std::endl;
        return;
    }

    // now we have access to some information about our file
    // since we read its header we can say what format (container) it's
    // and some other information related to the format itself.
    log << "format " << m_formatContext->iformat->name << ", duration " << m_formatContext->duration << " us, bit_rate " << m_formatContext->bit_rate << std::endl;

    log << "finding stream info from format" << std::endl;
    // read Packets from the Format to get stream information
    // this function populates pFormatContext->streams
    // (of size equals to pFormatContext->nb_streams)
    // the arguments are:
    // the AVFormatContext
    // and options contains options for codec corresponding to i-th stream.
    // On return each dictionary will be filled with options that were not found.
    // https://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#gad42172e27cddafb81096939783b157bb
    if (avformat_find_stream_info(m_formatContext,  NULL) < 0) {
        log << "ERROR could not get the stream info" << std::endl;
        return;
    }

    // the component that knows how to enCOde and DECode the stream
    // it's the codec (audio or video)
    // http://ffmpeg.org/doxygen/trunk/structAVCodec.html
    const AVCodec *pCodec = NULL;
    // this component describes the properties of a codec used by the stream i
    // https://ffmpeg.org/doxygen/trunk/structAVCodecParameters.html
    AVCodecParameters *pCodecParameters =  NULL;
    m_video_stream_index = -1;

    // loop though all the streams and print its main information
    for (int i = 0; i < m_formatContext->nb_streams; i++) {
        AVCodecParameters *pLocalCodecParameters =  NULL;
        pLocalCodecParameters = m_formatContext->streams[i]->codecpar;
        log << "AVStream->time_base before open coded " << m_formatContext->streams[i]->time_base.num << "/" << m_formatContext->streams[i]->time_base.den << std::endl;
        log << "AVStream->r_frame_rate before open coded " << m_formatContext->streams[i]->r_frame_rate.num << "/" << m_formatContext->streams[i]->r_frame_rate.den << std::endl;
        log << "AVStream->start_time " << m_formatContext->streams[i]->start_time << std::endl;
        log << "AVStream->duration " << m_formatContext->streams[i]->duration << std::endl;

        log << "finding the proper decoder (CODEC)" << std::endl;

        const AVCodec *pLocalCodec = NULL;

        // finds the registered decoder for a codec ID
        // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga19a0ca553277f019dd5b0fec6e1f9dca
        pLocalCodec = ::avcodec_find_decoder(pLocalCodecParameters->codec_id);

        if (pLocalCodec==NULL) {
            log << "ERROR unsupported codec!" << std::endl;
            // In this example if the codec is not found we just skip it
            continue;
        }

        // when the stream is a video we store its index, codec parameters and codec
        if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (m_video_stream_index == -1) {
                m_video_stream_index = i;
                pCodec = pLocalCodec;
                pCodecParameters = pLocalCodecParameters;
            }

            log << "Video Codec: resolution " << pLocalCodecParameters->width << " x " << pLocalCodecParameters->height << std::endl;
        } else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            log << "Audio Codec: " << pLocalCodecParameters->channels << " channels, sample rate " << pLocalCodecParameters->sample_rate << std::endl;
        }

        // print its name, id and bitrate
        log << "\tCodec " << pLocalCodec->name << " ID " << pLocalCodec->id << " bit_rate " << pLocalCodecParameters->bit_rate << std::endl;
    }

    if (m_video_stream_index == -1) {
        log << "File " << path << " does not contain a video stream!" << std::endl;
        return;
    }

    // https://ffmpeg.org/doxygen/trunk/structAVCodecContext.html
    if (!(m_codecContext = avcodec_alloc_context3(pCodec))) {
        log << "failed to allocated memory for AVCodecContext" << std::endl;
        return;
    }

    // Fill the codec context based on the values from the supplied codec parameters
    // https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#gac7b282f51540ca7a99416a3ba6ee0d16
    if (avcodec_parameters_to_context(m_codecContext, pCodecParameters) < 0)
    {
        log << "failed to copy codec params to codec context" << std::endl;
        return;
    }

    // Initialize the AVCodecContext to use the given AVCodec.
    // https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#ga11f785a188d7d9df71621001465b0f1d
    if (avcodec_open2(m_codecContext, pCodec, NULL) < 0)
    {
        log << "failed to open codec through avcodec_open2" << std::endl;
        return;
    }

    // https://ffmpeg.org/doxygen/trunk/structAVFrame.html
    if (!(m_frame = av_frame_alloc())) {
        log << "failed to allocated memory for AVFrame" << std::endl;
        return;
    }
    // https://ffmpeg.org/doxygen/trunk/structAVPacket.html
    if (!(m_packet = av_packet_alloc())) {
        log << "failed to allocated memory for AVPacket" << std::endl;
        return;
    }

    m_destinationFormat = AV_PIX_FMT_RGB32;
    m_destinationWidth = m_codecContext->width * scale;
    m_destinationHeight = m_codecContext->height * scale;
    m_imageConvertContext = sws_getContext(m_codecContext->width,
                                           m_codecContext->height,
                                           m_codecContext->pix_fmt,
                                           m_destinationWidth,
                                           m_destinationHeight,
                                           m_destinationFormat,
                                           SWS_BICUBIC,
                                           nullptr,
                                           nullptr,
                                           nullptr);
}

MP4Decoder::Frame &MP4Decoder::frame(std::size_t index,
                                     const e172::AbstractGraphicsProvider &graphicsProvider) const
{
    if(index < m_cache.size()) {
        return m_cache[index];
    }

    int response = 0;

    // fill the Packet with data from the Stream
    // https://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#ga4fdb3084415a82e3810de6ee60e46a61
    while (av_read_frame(m_formatContext, m_packet) >= 0) {
        // if it's the video stream
        if (m_packet->stream_index == m_video_stream_index) {
            m_log << "AVPacket->pts " << m_packet->pts << std::endl;
            response = decodePacket(m_cache,
                                    m_packet,
                                    m_codecContext,
                                    m_imageConvertContext,
                                    m_frame,
                                    m_log,
                                    m_destinationWidth,
                                    m_destinationHeight,
                                    m_destinationFormat,
                                    graphicsProvider);
            if (response < 0)
                break;

            if (index < m_cache.size()) {
                break;
            }
        }
        // https://ffmpeg.org/doxygen/trunk/group__lavc__packet.html#ga63d5a489b419bd5d45cfd09091cbcbc2
        av_packet_unref(m_packet);
    }

    if(index < m_cache.size()) {
        return m_cache[index];
    } else {
        m_log << "frame_count: " << frameCount() << std::endl;
        throw std::runtime_error("no frame for index: " + std::to_string(index));
    }
}

std::size_t MP4Decoder::frameCount() const
{
    return m_formatContext ? m_formatContext->streams[m_video_stream_index]->nb_frames - 1 : 0;
}

Rational<uint32_t> MP4Decoder::frameRate() const
{
    const auto rate = m_formatContext->streams[m_video_stream_index]->r_frame_rate;
    assert(rate.num >= 0);
    assert(rate.den >= 0);
    return Rational<uint32_t>::make(rate.num, rate.den).value();
}

MP4Decoder::~MP4Decoder()
{
    m_log << "releasing all the resources" << std::endl;
    if(m_formatContext) { avformat_close_input(&m_formatContext); }
    if(m_packet) { av_packet_free(&m_packet); }
    if(m_frame) { av_frame_free(&m_frame); }
    if(m_codecContext) { avcodec_free_context(&m_codecContext); }
    if(m_imageConvertContext) { sws_freeContext(m_imageConvertContext); }
}

} // namespace e172::impl::console::video_player
