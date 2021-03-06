//
// Created by ether on 2018/10/23.
//

#include "Decode.h"


#define null NULL
#define LOG_TAG "decode"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define MAX_AUDIO_FRAME_SIZE 44100*4
AudioPlayer mAudioPlayer;

void Decode::decode(const char *path, DECODE_TYPE decode_type) {
    av_register_all();
    pFmtCtx = avformat_alloc_context();
    if (avformat_open_input(&pFmtCtx, path, null, null) != 0) {
        LOGE("打开文件失败");
        return;
    }
    if (avformat_find_stream_info(pFmtCtx, null) < 0) {
        LOGE("获取文件信息失败");
        return;
    }
    index = -1;
    for (int i = 0; i < pFmtCtx->nb_streams; ++i) {
        switch (decode_type) {
            case DECODE_AUDIO:
                findIndex(DECODE_AUDIO);
                break;
            case DECODE_VIDEO:
                findIndex(DECODE_VIDEO);
                break;
            case DECODE_UNKNOWN:
                LOGE("未知类型");
                return;
            default:
                LOGE("暂不支持其他类型数据");
                return;
        }
    }
    if (index == -1) {
        LOGE("未找到对应的流信息");
        return;
    }
    pStream = pFmtCtx->streams[index];
    pCodec = avcodec_find_decoder(pStream->codecpar->codec_id);
    if (pCodec == null) {
        LOGE("未找到对应的解码器");
        return;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecCtx, pStream->codecpar);
    if (avcodec_open2(pCodecCtx, pCodec, null) < 0) {
        LOGE("无法打开解码器");
        return;
    }
    pPacket = static_cast<AVPacket *>(av_malloc(sizeof(AVPacket)));
    pFrame = av_frame_alloc();
    if (decode_type == DECODE_VIDEO) {
        video();
    } else {
        audio();
    }
}

void Decode::findIndex(DECODE_TYPE type) {
    AVMediaType mediaType;
    if (type == DECODE_VIDEO) {
        mediaType = AVMEDIA_TYPE_VIDEO;
    } else {
        mediaType = AVMEDIA_TYPE_AUDIO;
    }
    for (int i = 0; i < pFmtCtx->nb_streams; ++i) {
        if (pFmtCtx->streams[i]->codecpar->codec_type == mediaType) {
            index = i;
        }
    }
}

void Decode::audio() {
    SwrContext *swrContext = swr_alloc();
    enum AVSampleFormat inSampleFmt = pCodecCtx->sample_fmt;
    enum AVSampleFormat outSampleFmt = AV_SAMPLE_FMT_S16;

    int inSampleRate = pCodecCtx->sample_rate;
    int outSampleRate = 44100;

    uint64_t inSampleChannel = pCodecCtx->channel_layout;
    uint64_t outSampleChannel = AV_CH_LAYOUT_STEREO;

    swr_alloc_set_opts(mAudioPlayer.swrContext,
                       outSampleChannel,
                       outSampleFmt,
                       outSampleRate,
                       inSampleChannel,
                       inSampleFmt,
                       inSampleRate,
                       0,
                       null);
    swr_init(swrContext);
    int outChannelNum = av_get_channel_layout_nb_channels(outSampleChannel);
    int ret = -1;
    int dataSize;
    uint8_t *outBuffer = static_cast<uint8_t *>(av_malloc(MAX_AUDIO_FRAME_SIZE));
    while (av_read_frame(pFmtCtx, pPacket) >= 0) {
        if (pPacket->stream_index == index) {
            ret = avcodec_send_packet(pCodecCtx, pPacket);
            while (ret >= 0) {
                ret = avcodec_receive_frame(pCodecCtx, pFrame);
                if (ret == AVERROR(EAGAIN)) {
                    LOGE("%s", "读取解码数据失败");
                    break;
                } else if (ret == AVERROR_EOF) {
                    LOGE("%s", "解码完成");
//                    fclose(outFile);
                    break;
                } else if (ret < 0) {
                    LOGE("%s", "解码出错");
                    break;
                }
                mAudioPlayer.pushData(pFrame,pCodecCtx->channels,pCodecCtx->frame_size,pCodecCtx->sample_fmt,swrContext);
                mAudioPlayer.sampleFormat = pCodecCtx->sample_fmt;
                mAudioPlayer.channels = pCodecCtx->channels;
                mAudioPlayer.frameSize = pCodecCtx->frame_size;

//                int outBufferSize = av_samples_get_buffer_size(pFrame->linesize,
//                                                               pCodecCtx->channels,
//                                                               pCodecCtx->frame_size,
//                                                               pCodecCtx->sample_fmt,
//                                                               1);
//                int rst = swr_convert(swrCtx,
//                                      &outBuffer,
//                                      outBufferSize,
//                                      const_cast<const uint8_t **>(reinterpret_cast<uint8_t **>(pFrame->data)),
//                                      pFrame->nb_samples);
                //todo 将解码后数据填充进queue中
            }
        }
    }
    av_packet_unref(pPacket);
}

void Decode::video() {
    int df = 0;
    uint8_t *outputBuffer = static_cast<uint8_t *>(av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1)));
    av_image_fill_arrays(pFrame->data, pFrame->linesize, outputBuffer, AV_PIX_FMT_YUV420P,
                         pCodecCtx->width, pCodecCtx->height, 1);
    struct SwsContext *swsCtx = sws_getContext(
            pCodecCtx->width,
            pCodecCtx->height,
            pCodecCtx->pix_fmt,
            pCodecCtx->width,
            pCodecCtx->height,
            AV_PIX_FMT_YUV420P,
            SWS_BICUBIC,
            null,
            null,
            null
    );
    int ret;
    while (av_read_frame(pFmtCtx, pPacket) >= 0) {
        if (pPacket->stream_index == index) {
            ret = avcodec_send_packet(pCodecCtx, pPacket);
            while (ret >= 0) {
                ret = avcodec_receive_frame(pCodecCtx, pFrame);
                if (ret == AVERROR(EAGAIN)) {
                    LOGE("%s", "读取解码数据失败");
                    break;
                } else if (ret == AVERROR_EOF) {
                    LOGE("%s", "解码完成");
                    return;
                } else if (ret < 0) {
                    LOGE("%s", "解码出错");
                    break;
                }
                sws_scale(swsCtx,
                          pFrame->data,
                          pFrame->linesize,
                          0,
                          pCodecCtx->height,
                          pFrame->data,
                          pFrame->linesize);
                df++;
                LOGE("解码了%d帧", df);
                pFrame->width = pCodecCtx->width;
                pFrame->height = pCodecCtx->height;
//                blockQueue.push(pFrame);
                usleep(46000);
            }
        }
    }
}

void Decode::destroy() {
    avformat_close_input(&pFmtCtx);
    av_packet_unref(pPacket);
    av_frame_free(&pFrame);
    avcodec_free_context(&pCodecCtx);
    avformat_free_context(pFmtCtx);
}
