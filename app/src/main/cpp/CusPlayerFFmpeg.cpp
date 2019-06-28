//
// Created by zhangzd on 2019-06-26.
//


#include "CusPlayerFFmpeg.h"
#include "macro.h"

extern "C" {
#include "include/libavformat/avformat.h"
#include "include/libavutil/dict.h"
}



CusPlayerFFmpeg::CusPlayerFFmpeg(const char *dataSource, JavaCallHelper *pHelper) {
    url = new char[strlen(dataSource) + 1];
    strcpy(url, dataSource);
    this->javaCallHelper = pHelper;
}

void *prepareFFmpeg_(void *args) {
    CusPlayerFFmpeg *cusPlayerFFmpeg = static_cast<CusPlayerFFmpeg *>(args);
    cusPlayerFFmpeg->prepareFfmpeg();
    return 0;
}


void CusPlayerFFmpeg::prepareFfmpeg() {
    //执行在子线程中的方法，可以调用CusPlayerFFmpeg的成员
    avformat_network_init();
    avFormatContext = avformat_alloc_context();
    AVDictionary *opts = NULL;
    av_dict_set(&opts,"timeout","3000000",0);
    int ret = avformat_open_input(&avFormatContext,url,NULL,&opts);
    //判断打开是否成功，更新Java层
    if(ret != 0) {
        javaCallHelper->onError(THREAD_CHILD,FFMPEG_CAN_NOT_OPEN_URL);
        return;
    }
    //2. 查找流
    if(avformat_find_stream_info(avFormatContext,NULL) < 0) {
        if(javaCallHelper) {
            javaCallHelper->onError(THREAD_CHILD,FFMPEG_CAN_NOT_FIND_STREAMS);
        }
        return;
    }

    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        AVStream *stream = avFormatContext->streams[i];
        //获取解码器参数
        AVCodecParameters *codecpar = stream->codecpar;
        //通过解码器参数中的解码器ID ，生成解码器对象
        AVCodec * dec = avcodec_find_decoder(codecpar->codec_id);
        //未找到时返回NULL ，而NULL是个宏定义，其实就是0
        if(!dec){
            if(javaCallHelper) {
                javaCallHelper->onError(THREAD_CHILD,FFMPEG_FIND_DECODER_FAIL);
            }
            return;
        }

        //获取解码器上下文
        AVCodecContext * codecContext = avcodec_alloc_context3(dec);
        if(!codecContext) {
            if(javaCallHelper) {
                javaCallHelper->onError(THREAD_CHILD,FFMPEG_ALLOC_CODEC_CONTEXT_FAIL);
            }
            return;
        }
        //将解码器参数设置给解码器上下文
        //复制参数
        if (avcodec_parameters_to_context(codecContext, codecpar) < 0) {
            if (javaCallHelper)
                javaCallHelper->onError(THREAD_CHILD, FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL);
            return;
        }
        //打开解码器
        if (avcodec_open2(codecContext, dec, 0) != 0) {
            if (javaCallHelper)
                javaCallHelper->onError(THREAD_CHILD, FFMPEG_OPEN_DECODER_FAIL);
            return;
        }


        if(codecpar->codec_type ==  AVMEDIA_TYPE_AUDIO) {
            //视频
//            audioChannel = new AudioChannel(i,javaCallHelper,codecContext);
        }else if(codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
           //音频
            videoChannel = new VideoChannel(i,javaCallHelper,codecContext);
            videoChannel->setRenderFrame(renderFrame);
        }
    }
    if(!audioChannel && !videoChannel) {
        //视频处理和音频处理类都为空时，调用Java层报错，并退出
        if(javaCallHelper) {
            javaCallHelper->onError(THREAD_CHILD,FFMPEG_NOMEDIA);
        }
        return;
    }

    if(javaCallHelper) {
        javaCallHelper->onParpare(THREAD_CHILD);
    }
}

void CusPlayerFFmpeg::prepare() {
    pid_prepare = pthread_create(&pid_prepare,NULL,prepareFFmpeg_,this);
}

CusPlayerFFmpeg::~CusPlayerFFmpeg() {
}

void *startThread(void *args) {
    CusPlayerFFmpeg *cusPlayerFFmpeg = static_cast<CusPlayerFFmpeg *>(args);
    cusPlayerFFmpeg->play();
    return 0;
}


void CusPlayerFFmpeg::start() {
    isPlaying = true;
    if(videoChannel) {
        videoChannel->play();
    }
    if (audioChannel)
    {
//        audioChannel->play();
    }
    //开线程将流解析成packet，然后根据类型，放入到对应的音频视频packet队列中
    pthread_create(&pid_decode,NULL,startThread,this);

}

//真正的解码操作在此处实现
void CusPlayerFFmpeg::play() {
    int ret = 0;
    //死循环一直从流中读取数据，解析成packet
    while (isPlaying) {
        if(audioChannel && audioChannel->pkt_queue.size() > 100){
            av_usleep(1000 * 10);
            continue;
        }

        if(videoChannel && videoChannel->pkt_queue.size() > 100) {
            av_usleep(1000 * 10);
            continue;
        }

        AVPacket *avPacket = av_packet_alloc();
        ret = av_read_frame(avFormatContext,avPacket);
        if (ret == 0){
            //读取成功
            if(audioChannel && avPacket->stream_index == audioChannel->channelId) {
                audioChannel->pkt_queue.enQueue(avPacket);
            }else if(videoChannel && avPacket->stream_index == videoChannel->channelId) {
                videoChannel->pkt_queue.enQueue(avPacket);
            }

        } else if(ret == AVERROR_EOF) {
            //读取完毕 但是不一定播放完毕
            if (videoChannel && videoChannel->pkt_queue.empty() && videoChannel && videoChannel->frame_queue.empty() &&
                audioChannel->pkt_queue.empty() && audioChannel->frame_queue.empty()) {
                break;
            }
            //因为seek 的存在，就算读取完毕，依然要循环 去执行av_read_frame(否则seek了没用...)
        } else {
            break;
        }
    }

    //跳出循环代表播放结束，则设置标志位位false ，停止播放音频和视频
    isPlaying = false;
    if (videoChannel) {
        videoChannel->stop();
    }
    if(audioChannel) {
        audioChannel->stop();
    }

}

void CusPlayerFFmpeg::setRenderFrame(RenderFrame renderFrame) {
    this->renderFrame = renderFrame;

}

