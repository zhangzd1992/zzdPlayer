//
// Created by zhangzd on 2019-06-26.
//



#include "VideoChannel.h"

VideoChannel::VideoChannel(int id, JavaCallHelper *javaCallHelper, AVCodecContext *avCodecContext)
        : BaseChannel(id, javaCallHelper, avCodecContext) {

}


void * decode(void * args) {
    VideoChannel * videoChannel = static_cast<VideoChannel *>(args);
    videoChannel->decodePacket();
}



void VideoChannel::play() {

    pkt_queue.setWork(1);
    frame_queue.setWork(1);
    isPlaying = true;
    pthread_create(&pid_decode,NULL,decode,this);

}

void VideoChannel::stop() {

}

//将packet队列中的packet取出，解析成frame
void VideoChannel::decodePacket() {
    AVPacket *packet = 0;
    while(isPlaying) {


        int ret =  pkt_queue.deQueue(packet);
        avcodec_send_packet(avCodecContext,packet);

        AVFrame * avFrame = av_frame_alloc();
        avcodec_receive_frame(avCodecContext,avFrame);

        frame_queue.enQueue(avFrame);

        if(frame_queue.size() > 100) {
            av_usleep(1000 * 10);;
            break;
        }


    }

    isPlaying = false;

}


