#include "mp4_decoder.h"

mp4_decoder::mp4_decoder()
{

}

std::uint8_t mp4_decoder::canal(AVFrame *frame, std::size_t canal_no, std::size_t x, std::size_t y, std::size_t width) {
    return frame->data[canal_no][y * frame->linesize[canal_no] * frame->linesize[canal_no] / width + x * frame->linesize[canal_no] / width];
}

int mp4_decoder::decode(std::vector<pixel_primitives::bitmap>& dst_btmps, const char *path, std::ostream &log, std::size_t packet_count_to_process, double scale) {
    if (!path) {
        log << "You need to specify a media file." << std::endl;
        return -1;
    }

    log << "initializing all the containers, codecs and protocols." << std::endl;

    // AVFormatContext holds the header information from the format (Container)
    // Allocating memory for this component
    // http://ffmpeg.org/doxygen/trunk/structAVFormatContext.html
    AVFormatContext *pFormatContext = avformat_alloc_context();
    if (!pFormatContext) {
        log << "ERROR could not allocate memory for Format Context" << std::endl;
        return -1;
    }

    log << "opening the input file " << path << " and loading format (container) header" << std::endl;
    // Open the file and read its header. The codecs are not opened.
    // The function arguments are:
    // AVFormatContext (the component we allocated memory for),
    // url (filename),
    // AVInputFormat (if you pass NULL it'll do the auto detect)
    // and AVDictionary (which are options to the demuxer)
    // http://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#ga31d601155e9035d5b0e7efedc894ee49
    if (avformat_open_input(&pFormatContext, path, NULL, NULL) != 0) {
        log << "ERROR could not open the file" << std::endl;
        return -1;
    }

    // now we have access to some information about our file
    // since we read its header we can say what format (container) it's
    // and some other information related to the format itself.
    log << "format " << pFormatContext->iformat->name << ", duration " << pFormatContext->duration << " us, bit_rate " << pFormatContext->bit_rate << std::endl;

    log << "finding stream info from format" << std::endl;
    // read Packets from the Format to get stream information
    // this function populates pFormatContext->streams
    // (of size equals to pFormatContext->nb_streams)
    // the arguments are:
    // the AVFormatContext
    // and options contains options for codec corresponding to i-th stream.
    // On return each dictionary will be filled with options that were not found.
    // https://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#gad42172e27cddafb81096939783b157bb
    if (avformat_find_stream_info(pFormatContext,  NULL) < 0) {
        log << "ERROR could not get the stream info" << std::endl;
        return -1;
    }

    // the component that knows how to enCOde and DECode the stream
    // it's the codec (audio or video)
    // http://ffmpeg.org/doxygen/trunk/structAVCodec.html
    AVCodec *pCodec = NULL;
    // this component describes the properties of a codec used by the stream i
    // https://ffmpeg.org/doxygen/trunk/structAVCodecParameters.html
    AVCodecParameters *pCodecParameters =  NULL;
    int video_stream_index = -1;

    // loop though all the streams and print its main information
    for (int i = 0; i < pFormatContext->nb_streams; i++)
    {
        AVCodecParameters *pLocalCodecParameters =  NULL;
        pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
        log << "AVStream->time_base before open coded " << pFormatContext->streams[i]->time_base.num << "/" << pFormatContext->streams[i]->time_base.den << std::endl;
        log << "AVStream->r_frame_rate before open coded " << pFormatContext->streams[i]->r_frame_rate.num << "/" << pFormatContext->streams[i]->r_frame_rate.den << std::endl;
        log << "AVStream->start_time " << pFormatContext->streams[i]->start_time << std::endl;
        log << "AVStream->duration " << pFormatContext->streams[i]->duration << std::endl;

        log << "finding the proper decoder (CODEC)" << std::endl;

        AVCodec *pLocalCodec = NULL;

        // finds the registered decoder for a codec ID
        // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga19a0ca553277f019dd5b0fec6e1f9dca
        pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

        if (pLocalCodec==NULL) {
            log << "ERROR unsupported codec!" << std::endl;
            // In this example if the codec is not found we just skip it
            continue;
        }

        // when the stream is a video we store its index, codec parameters and codec
        if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (video_stream_index == -1) {
                video_stream_index = i;
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

    if (video_stream_index == -1) {
        log << "File " << path << " does not contain a video stream!" << std::endl;
        return -1;
    }

    // https://ffmpeg.org/doxygen/trunk/structAVCodecContext.html
    AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
    if (!pCodecContext) {
        log << "failed to allocated memory for AVCodecContext" << std::endl;
        return -1;
    }

    // Fill the codec context based on the values from the supplied codec parameters
    // https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#gac7b282f51540ca7a99416a3ba6ee0d16
    if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0)
    {
        log << "failed to copy codec params to codec context" << std::endl;
        return -1;
    }

    // Initialize the AVCodecContext to use the given AVCodec.
    // https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#ga11f785a188d7d9df71621001465b0f1d
    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0)
    {
        log << "failed to open codec through avcodec_open2" << std::endl;
        return -1;
    }

    // https://ffmpeg.org/doxygen/trunk/structAVFrame.html
    AVFrame *pFrame = av_frame_alloc();
    if (!pFrame)
    {
        log << "failed to allocated memory for AVFrame" << std::endl;
        return -1;
    }
    // https://ffmpeg.org/doxygen/trunk/structAVPacket.html
    AVPacket *pPacket = av_packet_alloc();
    if (!pPacket)
    {
        log << "failed to allocated memory for AVPacket" << std::endl;
        return -1;
    }

    int response = 0;

    // fill the Packet with data from the Stream
    // https://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#ga4fdb3084415a82e3810de6ee60e46a61
    while (av_read_frame(pFormatContext, pPacket) >= 0)
    {
        // if it's the video stream
        if (pPacket->stream_index == video_stream_index) {
            log << "AVPacket->pts " << pPacket->pts << std::endl;
            response = decode_packet(dst_btmps, pPacket, pCodecContext, pFrame, log, scale);
            if (response < 0)
                break;
            // stop it, otherwise we'll be saving hundreds of frames
            if (--packet_count_to_process <= 0) break;
        }
        // https://ffmpeg.org/doxygen/trunk/group__lavc__packet.html#ga63d5a489b419bd5d45cfd09091cbcbc2
        av_packet_unref(pPacket);
    }

    log << "releasing all the resources" << std::endl;

    avformat_close_input(&pFormatContext);
    av_packet_free(&pPacket);
    av_frame_free(&pFrame);
    avcodec_free_context(&pCodecContext);
    return 0;
}

int mp4_decoder::decode_packet(std::vector<pixel_primitives::bitmap> &dst_btmps, AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame, std::ostream &log, double scale)
{
    // Supply raw packet data as input to a decoder
    // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
    int response = avcodec_send_packet(pCodecContext, pPacket);

    if (response < 0) {
        log << "Error while sending a packet to the decoder: " << av_err2str(response) << std::endl;
        return response;
    }

    while (response >= 0)
    {
        // Return decoded output data (into a frame) from a decoder
        // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
        response = avcodec_receive_frame(pCodecContext, pFrame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
            break;
        } else if (response < 0) {
            log << "Error while receiving a frame from the decoder: " << av_err2str(response) << std::endl;
            return response;
        }

        if (response >= 0) {
            bool log_frame = true;

            if(log_frame) {
                log
                        << "Frame " << pCodecContext->frame_number
                        << " (type=" << av_get_picture_type_char(pFrame->pict_type)
                        << ", size=" << pFrame->pkt_size
                        << " bytes, format=" << av_get_sample_fmt_name(AVSampleFormat(pFrame->format))
                        << ") pts " << pFrame->pts
                        << " key_frame " << pFrame->key_frame
                        << " [DTS " << pFrame->coded_picture_number << "]" << std::endl;
            }



            char frame_filename[1024];
            snprintf(frame_filename, sizeof(frame_filename), "%s-%d.pgm", "frame", pCodecContext->frame_number);
            // Check if the frame is a planar YUV 4:2:0, 12bpp
            // That is the format of the provided .mp4 file
            // RGB formats will definitely not give a gray image
            // Other YUV image may do so, but untested, so give a warning
            if (pFrame->format != AV_PIX_FMT_YUV420P)
            {
                log << "Warning: the generated file may not be a grayscale image, but could e.g. be just the R component if the video format is RGB" << std::endl;
            }
            // save a grayscale frame into a .pgm file

            if(log_frame) {
                log << "\tw: " << pFrame->width << ", h: " << pFrame->height << std::endl;
                log << "\tlen 0: " << pFrame->linesize[0] << std::endl;
                log << "\tlen 1: " << pFrame->linesize[1] << std::endl;
                log << "\tlen 2: " << pFrame->linesize[2] << std::endl;
                log << "\tlen 3: " << pFrame->linesize[3] << std::endl;
                log << "\tlen 4: " << pFrame->linesize[4] << std::endl;
                log << "\tlen 5: " << pFrame->linesize[5] << std::endl;
                log << "\tlen 6: " << pFrame->linesize[6] << std::endl;
                log << "\tlen 7: " << pFrame->linesize[7] << std::endl;
            }

            const auto scaled_w = pFrame->width * scale;
            const auto scaled_h = pFrame->height * scale;

            dst_btmps.push_back(pixel_primitives::bitmap {
                                    new std::uint32_t[scaled_w * scaled_h],
                                    static_cast<size_t>(scaled_w),
                                    static_cast<size_t>(scaled_h)
                                });

            for(std::size_t y = 0; y < scaled_h; ++y) {
                for(std::size_t x = 0; x < scaled_w; ++x) {
                    const auto g = canal(pFrame, 0, x, y, scaled_w);
                    const auto b = canal(pFrame, 1, x, y, scaled_w);
                    const auto r = canal(pFrame, 2, x, y, scaled_w);
                    //log << "c: 0x" << std::hex << canal << std::dec << " [ " << x << ", " << y << " ]" << std::endl;
                    pixel_primitives::pixel(dst_btmps.back(), x, y)
                            = 0xff000000
                            | std::uint32_t(r) << 16
                            | std::uint32_t(g) << 8
                            | std::uint32_t(b) << 0;
                }
            }

            save_gray_frame(pFrame->data[0], pFrame->linesize[0], pFrame->width, pFrame->height, frame_filename, log);
        }
    }
    return 0;
}

void mp4_decoder::save_gray_frame(std::uint8_t *buf, int wrap, int xsize, int ysize, char *filename, std::ostream &log) {
    FILE *f;
    int i;
    f = fopen(filename,"w");
    // writing the minimal required header for a pgm file format
    // portable graymap format -> https://en.wikipedia.org/wiki/Netpbm_format#PGM_example
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);

    // writing line by line
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}
