//
// Created by ether on 2018/10/23.
//

#ifndef BAMBOO_DECODE_H
#define BAMBOO_DECODE_H


#include "BlockQueue.h"
#include <android/log.h>

extern "C" {
#include "../jni/includes/libavutil/frame.h"
#include "../jni/includes/libavcodec/avcodec.h"
#include "../jni/includes/libavformat/avformat.h"
};
enum DECODE_TYPE {
    DECODE_VIDEO, DECODE_AUDIO, DECODE_UNKNOWN
};

class Decode {
public:
    void decode(const char *path, DECODE_TYPE decode_type, BlockQueue &blockQueue);
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

    void audio(BlockQueue &blockQueue);

    void video(BlockQueue &blockQueue);
};


#endif //BAMBOO_DECODE_H
