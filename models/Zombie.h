//
// Created by 幻弦让叶 on 2023/10/5.
//

#ifndef PVZ_ZOMBIE_H
#define PVZ_ZOMBIE_H

struct Zombie {
    int x, y;
    int row;
    int frameIndex;
    bool isUsed;
    int speed;
};

#endif //PVZ_ZOMBIE_H
