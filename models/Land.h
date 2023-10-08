//
// Created by 幻弦让叶 on 2023/10/4.
//

#ifndef PVZ_LAND_H
#define PVZ_LAND_H

struct Land{

    //0 -> 没有植物
    int type;
    //帧下标
    int frameIndex;
    //植物是否被捕获
    bool caught;
    //植物死亡计数
    int deadTime;

    //block定时器
    int timer;
    //block坐标
    int x, y;
};

#endif //PVZ_LAND_H
