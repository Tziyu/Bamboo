//
// Created by ether on 2018/10/23.
//

#ifndef BAMBOO_DECODE_H
#define BAMBOO_DECODE_H


#include <android/log.h>
#include <cstdint>
#include <unistd.h>
#include "../util/BlockQueue.h"
#include "../audio/AudioPlayer.h"

extern "C" {
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
};
enum DECODE_TYPE {
    DECODE_VIDEO, DECODE_AUDIO, DECODE_UNKNOWN
};

class Decode {
public:
    void decode(const char *path, DECODE_TYPE decode_type);

    void destroy();

private:
    AVCodecContext *pCodecCtx;
    AVFormatContext *pFmtCtx;
    AVPacket *pPacket;
    int index;
    AVStream *pStream;
    AVCodec *pCodec;
    AVFrame *pFrame;

    void findIndex(DECODE_TYPE type);

    void audio();

    void video();
};


#endif //BAMBOO_DECODE_H
