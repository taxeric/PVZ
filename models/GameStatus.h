//
// Created by 幻弦让叶 on 2023/10/8.
//

#ifndef PVZ_GAMESTATUS_H
#define PVZ_GAMESTATUS_H

#include <easyx.h>
#include "Plant.h"

enum {
    GameIdle,
    GameRunning,
    GameSuccess,
    GameFailed,
};

struct GameStatus {
    /**
     * 关卡
     */
    int level = 0;

    /**
     * 生成的僵尸数量
     */
    int zombieCount;

    /**
     * 关卡最大僵尸数量
     */
    int zombieMaxCount;

    /**
     * 一大波僵尸来袭次数
     */
    int tooManyZombies;

    /**
     * 每次一大波僵尸来袭的僵尸数量
     */
    int tooManyZombieCount;

    /**
     * 已经凉了的僵尸数量
     */
    int killCount;

    /**
     * 阳光数量
     */
    int sunshineBalls;

    /**
     * 卡槽图片, 最多6个
     */
    IMAGE imgCardsPics[6] = { 0 };

    /**
     * 卡槽图片对应的植物, 最多6个, 每个植物最多有21幅图片, 用于切换显示摇摆~
     */
    IMAGE* imgPlantsPics[6][21] = { nullptr };

    /**
     * 所选植物
     */
    Plant* plants[6] = { nullptr };

    /**
     * 本次所选植物数量
     */
    int plantCount;

    /**
     * 关卡状态
     */
    int levelStatus;
};

#endif //PVZ_GAMESTATUS_H
