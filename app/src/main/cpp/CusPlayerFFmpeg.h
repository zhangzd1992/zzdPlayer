//
// Created by zhangzd on 2019-06-26.
//

#ifndef CUSPLAYER_CUSPLAYERFFMPEG_H
#define CUSPLAYER_CUSPLAYERFFMPEG_H
#include <pthread.h>
#include "JavaCallHelper.h"


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

private:
    char * url;
    pthread_t pid_prepare;    //初始化ffmpeg 线程索引
    JavaCallHelper *javaCallHelper;

};


#endif //CUSPLAYER_CUSPLAYERFFMPEG_H
