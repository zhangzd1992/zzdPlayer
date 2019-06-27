//
// Created by 张兆东 on 2019-06-27.
//

#ifndef ZZDPLAYER_BASECHANNEL_H
#define ZZDPLAYER_BASECHANNEL_H


#include "safe_queue.h"
#include "JavaCallHelper.h"

extern "C" {
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/frame.h"
};

class BaseChannel {
public:
    BaseChannel(int id,JavaCallHelper *javaCallHelper,AVCodecContext * avCodecContext)
        :channelId(id),
        javaCallHelper(javaCallHelper),
        avCodecContext(avCodecContext) {

        //设置栈销毁回调
        pkt_queue.setReleaseHandle(releaseAvPacket);
        frame_queue.setReleaseHandle(releaseAvFrame);
    };

    static void releaseAvPacket(AVPacket *&packet) {
        if(packet){
            av_packet_free(&packet);
            packet = 0;
        }
    }

    static void releaseAvFrame(AVFrame *&frame) {
        if(frame) {
            av_frame_free(&frame);
            frame = 0;
        }
    }


    virtual void play() =0;
    virtual void stop() =0 ;

    SafeQueue<AVPacket *> pkt_queue;
    SafeQueue<AVFrame  *> frame_queue;
    AVCodecContext *avCodecContext;
    JavaCallHelper *javaCallHelper;
    volatile int channelId;
    void bool isPlaying;

    virtual ~BaseChannel(){
        if(avCodecContext) {
            avcodec_close(avCodecContext);
            avcodec_free_context(&avCodecContext);
            avCodecContext = 0;
        }
        pkt_queue.clear();
        frame_queue.clear();
    }


};


#endif //ZZDPLAYER_BASECHANNEL_H
