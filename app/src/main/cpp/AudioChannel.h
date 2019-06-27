//
// Created by zhangzd on 2019-06-26.
//

#ifndef CUSPLAYER_AUDIOCHANNEL_H
#define CUSPLAYER_AUDIOCHANNEL_H


#include "BaseChannel.h"

class AudioChannel: BaseChannel {
public:
    AudioChannel(int id, JavaCallHelper *javaCallHelper, AVCodecContext *avCodecContext);

};


#endif //CUSPLAYER_AUDIOCHANNEL_H
