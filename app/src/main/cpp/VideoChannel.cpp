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
    return 0;
}

//渲染线程回调方法
void *render(void *args) {
    VideoChannel * channel = static_cast<VideoChannel *>(args);
    channel->synchronizeFrame();
    return 0;
}


void VideoChannel::play() {

    pkt_queue.setWork(1);
    frame_queue.setWork(1);
    isPlaying = true;

    //创建解码线程
    pthread_create(&pid_decode,NULL,decode,this);
    //创建渲染线程
    pthread_create(&pid_renering,NULL,render,this);

}

void VideoChannel::stop() {

}

//将packet队列中的packet取出，解析成frame
void VideoChannel::decodePacket() {
    AVPacket *packet = 0;
    while(isPlaying) {
        int ret =  pkt_queue.deQueue(packet);
        if (!isPlaying) {
            break;
        }
        if (ret) {
            //取数据失败，则停止本次循环，继续取
            continue;
        }
        avcodec_send_packet(avCodecContext,packet);
        releaseAvPacket(packet);  //释放packet
        if(ret == AVERROR(EAGAIN)) {
            //队列暂时没有数据，则挂起
            continue;
        }else if(ret < 0) {
            //失败
            break;
        }


        AVFrame * avFrame = av_frame_alloc();
        avcodec_receive_frame(avCodecContext,avFrame);
        frame_queue.enQueue(avFrame);
        while (frame_queue.size() > 100 && isPlaying) {
            av_usleep(1000 * 10);
            continue;
        }

    }
    releaseAvPacket(packet);
}

//将frame数据转换为rgba 格式
void VideoChannel::synchronizeFrame() {

    SwsContext * swsContext = sws_getContext(avCodecContext->width,avCodecContext->height,avCodecContext->pix_fmt,avCodecContext->width,avCodecContext->height,AV_PIX_FMT_RGBA,SWS_BILINEAR,0,0,0);



    uint8_t *dst_data[4]; //argb
    int dst_linesize[4];
    AVFrame *frame = NULL;
    while (isPlaying) {
        int ret = frame_queue.deQueue(frame);
        if (!isPlaying) {
            break;
        }
        if(!ret) {
            continue;
        }

        sws_scale(swsContext,frame->data,frame->linesize,0,frame->height,dst_data,dst_linesize);

        //数据渲染到nativeWindow上
        if(renderFrame) {
            renderFrame(dst_data[0],dst_linesize[0],frame->width,frame->height);
        }

        av_usleep(16 * 1000);

        releaseAvFrame(frame);
    }


    av_freep(&dst_data[0]);
    isPlaying = false;
    releaseAvFrame(frame);
    sws_freeContext(swsContext);
}

void VideoChannel::setRenderFrame(RenderFrame renderFrame) {
    this->renderFrame = renderFrame;
}


