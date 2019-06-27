//
// Created by zhangzd on 2019-06-26.
//

#ifndef CUSPLAYER_VIDEOCHANNEL_H
#define CUSPLAYER_VIDEOCHANNEL_H


#include "BaseChannel.h"
#include <pthread.h>


extern "C" {
#include <libavutil/time.h>
}

class VideoChannel : BaseChannel {
public:
    VideoChannel(int id, JavaCallHelper *javaCallHelper, AVCodecContext *avCodecContext);

    virtual void play();
    virtual void stop();

    void decodePacket();

private:
    pthread_t pid_decode;  //解码子线程的索引
};


#endif //CUSPLAYER_VIDEOCHANNEL_H
