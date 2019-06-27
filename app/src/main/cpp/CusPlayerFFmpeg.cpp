//
// Created by zhangzd on 2019-06-26.
//

#include <opencl-c.h>
#include "CusPlayerFFmpeg.h"
#include "macro.h"

extern "C" {
#include "include/libavformat/avformat.h"
#include "include/libavutil/dict.h"
}


void *prepareFFmpeg_(void *args) {
    CusPlayerFFmpeg *cusPlayerFFmpeg = static_cast<CusPlayerFFmpeg *>(args);
    cusPlayerFFmpeg->prepareFfmpeg();
}


void CusPlayerFFmpeg::prepareFfmpeg() {
    //执行在子线程中的方法，可以调用CusPlayerFFmpeg的成员
    avformat_network_init();
    AVFormatContext *avFormatContext = avformat_alloc_context();
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


        if(codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频



        }else if(codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
           //音频




        }
    }
}

void CusPlayerFFmpeg::prepare() {
    pid_prepare = pthread_create(&pid_prepare,NULL,prepareFFmpeg_,this);
}

CusPlayerFFmpeg::~CusPlayerFFmpeg() {

}

CusPlayerFFmpeg::CusPlayerFFmpeg(const char *dataSource, JavaCallHelper *pHelper) {
    url = new char[strlen(dataSource) + 1];
    strcpy(url, dataSource);
    this->javaCallHelper = pHelper;
}
