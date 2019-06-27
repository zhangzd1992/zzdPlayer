//
// Created by zhangzd on 2019-06-26.
//

#ifndef CUSPLAYER_CUSPLAYERFFMPEG_H
#define CUSPLAYER_CUSPLAYERFFMPEG_H
#include <pthread.h>
#include "JavaCallHelper.h"
#include "VideoChannel.h"
#include "AudioChannel.h"


extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/time.h>
}

class CusPlayerFFmpeg {
public:


    CusPlayerFFmpeg(const char *dataSource, JavaCallHelper *pHelper);
    virtual ~CusPlayerFFmpeg();

    void prepareFfmpeg();
    void prepare();

    void start();

    void play();

private:
    char * url;
    pthread_t pid_prepare;    //初始化ffmpeg 线程索引
    pthread_t pid_decode;     //解码子线程索引
    JavaCallHelper *javaCallHelper;
    VideoChannel *videoChannel;
    AudioChannel *audioChannel;
    bool isPlaying;
    AVFormatContext *avFormatContext;


};


#endif //CUSPLAYER_CUSPLAYERFFMPEG_H
