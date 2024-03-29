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
    BaseChannel(int id,JavaCallHelper *javaCallHelper,AVCodecContext * avCodecContext,AVRational time_base)
        :channelId(id),
        javaCallHelper(javaCallHelper),
        avCodecContext(avCodecContext),
        time_base(time_base){

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
    virtual ~BaseChannel(){
        if(avCodecContext) {
            avcodec_close(avCodecContext);
            avcodec_free_context(&avCodecContext);
            avCodecContext = 0;
        }
        pkt_queue.clear();
        frame_queue.clear();
    }
    virtual void play() =0;
    virtual void stop() =0 ;
    void stopWork() {
        frame_queue.setWork(0);
        pkt_queue.setWork(0);
    }

    void startWork() {
        frame_queue.setWork(1);
        pkt_queue.setWork(1);
    }

    void clear() {
        frame_queue.clear();
        pkt_queue.clear();
    }


    SafeQueue<AVPacket *> pkt_queue;
    SafeQueue<AVFrame *> frame_queue;
    volatile int channelId;
    volatile bool isPlaying  ;

    AVCodecContext *avCodecContext;
    JavaCallHelper *javaCallHelper;
    AVRational time_base;
    double clock = 0;


};


#endif //ZZDPLAYER_BASECHANNEL_H
