//
// Created by zhangzd on 2019-06-26.
//




#include "VideoChannel.h"



void dropFrame(queue<AVFrame *> &q) {
    if (!q.empty()) {
        AVFrame *frame = q.front();
        q.pop();
        BaseChannel::releaseAvFrame(frame);
    }
}
VideoChannel::VideoChannel(int id, JavaCallHelper *javaCallHelper, AVCodecContext *avCodecContext,
                           AVRational timeBase)
        : BaseChannel(id, javaCallHelper, avCodecContext,timeBase) {

//  因为在父类中已经进行了赋值操作，此处不用在进行赋值
//    this->javaCallHelper = javaCallHelper;
//    this->avCodecContext = avCodecContext;


    frame_queue.setReleaseHandle(releaseAvFrame);
    frame_queue.setSyncHandle(dropFrame);

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
    isPlaying = 0;
}

//将packet队列中的packet取出，解析成frame
void VideoChannel::decodePacket() {
    AVPacket *packet = 0;
    while(isPlaying) {
        int ret =  pkt_queue.deQueue(packet);
        if (!isPlaying) {
            break;
        }
        if (!ret) {
            //取数据失败，则停止本次循环，继续取
            continue;
        }
        ret = avcodec_send_packet(avCodecContext,packet);
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
    SwsContext *swsContext = sws_getContext(
            avCodecContext->width, avCodecContext->height, avCodecContext->pix_fmt,
            avCodecContext->width, avCodecContext->height, AV_PIX_FMT_RGBA,
            SWS_BILINEAR, 0, 0, 0);


    uint8_t *dst_data[4]; //argb
    int dst_linesize[4];
    av_image_alloc(dst_data, dst_linesize,
                   avCodecContext->width, avCodecContext->height, AV_PIX_FMT_RGBA, 1);
    AVFrame *frame = NULL;
    while (isPlaying) {
        int ret = frame_queue.deQueue(frame);
        if (!isPlaying) {
            break;
        }
        if(!ret) {
            continue;
        }

        sws_scale(swsContext,reinterpret_cast<const uint8_t *const *>(frame->data),frame->linesize,0,frame->height,dst_data,dst_linesize);

        //数据渲染到nativeWindow上
        if(renderFrame) {
            renderFrame(dst_data[0],dst_linesize[0],avCodecContext->width,avCodecContext->height);
        }

        if (audioChannel) {

            clock = frame->pts * av_q2d(time_base);
//        解码时间算进去
            double frame_delays = 1.0 / fps;
            double audioClock = audioChannel->clock;
//        将解码所需要的时间算进去  因为配置差的手机 解码耗时需要多一些
            double extra_delay = frame->repeat_pict / (2 * fps);
            double delay = extra_delay+frame_delays;

            double diff = clock - audioClock;
            LOGE("----相差-------%d ",diff);
//        视频超前  1  延后
            if (clock > audioClock) {
//        视频超前
                if (diff > 1) {
                    //差的太久了， 那只能慢慢赶 不然就是卡好久
                    av_usleep((delay * 2) * 1000000);
                } else{
                    av_usleep((delay + diff) * 1000000);
                }
            } else{
//        视频延后 音频超前
                if (diff > -1) {
//
//                不休眠
                } else if (diff >= -0.05) {
//                救一下
//视频需要追赶     丢帧  同步
                    releaseAvFrame(frame);
                    frame_queue.sync();
//                减少延迟时间
                    //执行同步操作 删除到最近的key frame
                } else{


                }


            }
        }
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

void VideoChannel::setFps(int fps) {
    this->fps = fps;
}

void VideoChannel::setAudioChannel(AudioChannel *audioChannel) {
    this->audioChannel = audioChannel;
}

VideoChannel::~VideoChannel() {

}


