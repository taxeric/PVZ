//
// Created by 幻弦让叶 on 2023/10/7.
//

#ifndef PVZ_BULLET_H
#define PVZ_BULLET_H

struct Bullet {

    //子弹坐标
    int x, y;
    //行
    int row;
    //是否使用
    bool isUsed;
    //速度
    int speed;
};

#endif //PVZ_BULLET_H
