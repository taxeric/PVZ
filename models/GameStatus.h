//
// Created by 幻弦让叶 on 2023/10/8.
//

#ifndef PVZ_GAMESTATUS_H
#define PVZ_GAMESTATUS_H

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
    int level;

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
     * 关卡状态
     */
    int levelStatus;
};

#endif //PVZ_GAMESTATUS_H