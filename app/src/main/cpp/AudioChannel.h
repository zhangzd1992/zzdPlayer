//
// Created by zhangzd on 2019-06-26.
//

#ifndef CUSPLAYER_AUDIOCHANNEL_H
#define CUSPLAYER_AUDIOCHANNEL_H


#include "BaseChannel.h"
#include <SLES/OpenSLES_Android.h>
extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/time.h>
};

class AudioChannel: public BaseChannel {
public:
    AudioChannel(int id, JavaCallHelper *javaCallHelper, AVCodecContext *avCodecContext,AVRational baseTime);
    virtual void play();

    virtual void stop();
    void initOpenSL();



private:
    int out_channels;      //输出数据的通道数
    int out_samplesize;    //输出数据的采样位数
    int out_sample_rate;   //输出音频的采样率
    pthread_t pid_init;
    pthread_t pid_audio_decode;
    SwrContext * swr_ctx;

public:
    uint8_t *buffer;


    void decode();

    int getPcm();
};


#endif //CUSPLAYER_AUDIOCHANNEL_H
