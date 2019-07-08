//
// Created by zhangzd on 2019-06-26.
//

#ifndef CUSPLAYER_VIDEOCHANNEL_H
#define CUSPLAYER_VIDEOCHANNEL_H


#include "BaseChannel.h"
#include "AudioChannel.h"
#include <pthread.h>


extern "C" {
#include "libavutil/time.h"
#include "libswscale/swscale.h"
#include <libavutil/imgutils.h>
}
typedef void (*RenderFrame)(uint8_t *,int ,int ,int);
class VideoChannel : public BaseChannel {
public:
    VideoChannel(int id, JavaCallHelper *javaCallHelper, AVCodecContext *avCodecContext,
                     AVRational rational);
    ~VideoChannel();

    virtual void play();
    virtual void stop();

    void decodePacket();

    void synchronizeFrame();
    void setRenderFrame(RenderFrame renderFrame);
    void setFps(int fps);

    void setAudioChannel(AudioChannel *audioChannel);

private:
    pthread_t pid_decode;  //解码子线程的索引
    pthread_t pid_renering; //渲染线程索引
    RenderFrame renderFrame;
    int fps;
    AudioChannel *audioChannel = 0;
};


#endif //CUSPLAYER_VIDEOCHANNEL_H
