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
    int hp;
    bool eating;
    bool freeze;
    int freezeTimer;
    bool head;
    bool lostHead;
    bool dead;

    /**
     * 当前攻击的植物行列
     */
     int attackRow;
     int attackColumn;
};

#endif //PVZ_ZOMBIE_H
