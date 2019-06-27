//
// Created by zhangzd on 2019-06-26.
//

#include "AudioChannel.h"

AudioChannel::AudioChannel(int id, JavaCallHelper *javaCallHelper, AVCodecContext *avCodecContext)
        : BaseChannel(id, javaCallHelper, avCodecContext) {}
