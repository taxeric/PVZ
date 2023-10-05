//
// Created by 幻弦让叶 on 2023/10/5.
//

#ifndef PVZ_SUNSHINEBALL_H
#define PVZ_SUNSHINEBALL_H

struct SunshineBall {

    //飘落过程的x
    int x;
    //飘落过程的y
    int y;
    //落点y
    int destY;
    //帧
    int frameIndex;
    //是否使用
    bool isUsed;
    //
    int timer;
};

#endif //PVZ_SUNSHINEBALL_H
