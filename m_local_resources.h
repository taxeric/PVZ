//
// Created by 幻弦让叶 on 2023/10/4.
//

#ifndef PVZ_M_LOCAL_RESOURCES_H
#define PVZ_M_LOCAL_RESOURCES_H

enum {
    /**
     * 向日葵
     */
    SUNFLOWER,

    /**
     * 豌豆射手
     */
    PEASHOOT,

    /**
     * 土豆地雷
     */
    POTATOMINE,

    /**
     * 火爆辣椒
     */
//    JALAPENO,

    /**
     * 食人花
     */
//    CHOMPER,

    /**
     * 双发射手
     */
    REPEATERPEA,
    PLANTS_COUNT,
};

enum {

    /**
     * 下落
     */
    SUNSHINE_DOWN,

    /**
     * 落到地上
     */
    SUNSHINE_GROUND,

    /**
     * 收集
     */
    SUNSHINE_COLLECT,

    /**
     * 向日葵生成
     */
    SUNSHINE_PRODUCT
};

#define BASE_RES_PATH "../res/graphics/"

#define BASE_RES_START_MENU_PATH "../res/graphics/Screen/MainMenu.png"
#define BASE_RES_ADVENTURE_0_PATH "../res/graphics/Screen/Adventure_0.png"
#define BASE_RES_ADVENTURE_1_PATH "../res/graphics/Screen/Adventure_1.png"

#define BASE_RES_BG_PATH "../res/graphics/Items/Background/Background_0.jpg"
#define BASE_RES_BAR_BG_PATH "../res/graphics/Screen/SeedBank.png"
#define BASE_RES_CHOOSE_PLANTS_PATH "../res/graphics/Screen/PanelBackground.png"
#define BASE_RES_CHOOSE_PLANTS_START_BTN_PATH "../res/graphics/Screen/StartButton.png"

//阳光路径
#define RES_PIC_SUNSHINE_PATH "../res/graphics/Plants/Sun/"

//向日葵卡槽
#define RES_CARD_PIC_SUNFLOWER "../res/graphics/Cards/card_sunflower.png"
//向日葵图片路径
#define RES_PIC_SUNFLOWER_PATH "../res/graphics/Plants/SunFlower/"
//豌豆射手卡槽
#define RES_CARD_PIC_PEASHOOTER "../res/graphics/Cards/card_peashooter.png"
//豌豆射手图片路径
#define RES_PIC_PEASHOOTER_PATH "../res/graphics/Plants/Peashooter/"
//土豆地雷卡槽
#define RES_CARD_PIC_POTATOMINE "../res/graphics/Cards/card_potatomine.png"
//土豆地雷idle图片路径
#define RES_PIC_POTATOMINE_PATH "../res/graphics/Plants/PotatoMine/PotatoMine/"
//火爆辣椒卡槽
#define RES_CARD_PIC_JALAPENO "../res/graphics/Cards/card_jalapeno.png"
//火爆辣椒放置图片路径
#define RES_PIC_JALAPENO_PATH "../res/graphics/Plants/Jalapeno/Jalapeno/"
//火爆辣椒释放图片路径
#define RES_PIC_JALAPENO_RELEASE_PATH "../res/graphics/Plants/Jalapeno/JalapenoExplode/"
//食人花卡槽
#define RES_CARD_PIC_CHOMPER "../res/graphics/Cards/card_chomper.png"
//食人花图片路径
#define RES_PIC_CHOMPER_PATH "../res/graphics/Plants/Chomper/Chomper/"
//食人花攻击图片路径
#define RES_PIC_CHOMPER_ATTACK_PATH "../res/graphics/Plants/Chomper/ChomperAttack/"
//食人花咽下去图片路径
#define RES_PIC_CHOMPER_DIGEST_PATH "../res/graphics/Plants/Chomper/ChomperDigest/"
//双发射手卡槽
#define RES_CARD_PIC_REPEATERPEA "../res/graphics/Cards/card_repeaterpea.png"
//向日葵图片路径
#define RES_PIC_REPEATERPEA_PATH "../res/graphics/Plants/RepeaterPea/"

//普通僵尸行走图片路径
#define RES_PIC_NORMAL_ZOMBIE_WALK_PATH "../res/graphics/Zombies/NormalZombie/Zombie/"
//普通僵尸站立图片路径
#define RES_PIC_NORMAL_ZOMBIE_STAND_PATH "../res/graphics/Zombies/NormalZombie/ZombieStand/"
//普通僵尸攻击图片路径
#define RES_PIC_NORMAL_ZOMBIE_ATTACK_PATH "../res/graphics/Zombies/NormalZombie/ZombieAttack/"
//普通僵尸头掉了图片路径
#define RES_PIC_NORMAL_ZOMBIE_HEAD_PATH "../res/graphics/Zombies/NormalZombie/ZombieHead/"
//普通僵尸头掉了身体图片路径
#define RES_PIC_NORMAL_ZOMBIE_LOST_HEAD_PATH "../res/graphics/Zombies/NormalZombie/ZombieLostHead/"
//普通僵尸死亡图片路径
#define RES_PIC_NORMAL_ZOMBIE_DIE_PATH "../res/graphics/Zombies/NormalZombie/ZombieDie/"
//普通僵尸因爆炸死亡图片路径
#define RES_PIC_NORMAL_ZOMBIE_BOOM_DIE_PATH "../res/graphics/Zombies/NormalZombie/BoomDie/"

//正常豌豆子弹图片
#define RES_PIC_BULLET_PEA_NORMAL "../res/graphics/Bullets/PeaNormal/ProjectilePea.png"
//碰撞僵尸后的豌豆子弹图片
#define RES_PIC_BULLET_PEA_NORMAL_EXPLODE "../res/graphics/Bullets/PeaNormalExplode/PeaNormalExplode_0.png"

void loadSunshineBallPics(int size);

/**
 * 普通僵尸行走
 * @param size
 */
void loadNormalZombieWalkPics(int size);

/**
 * 普通僵尸站立
 */
 void loadNormalZombieStandPics(int size);

/**
 * 普通僵尸攻击
 * @param size
 */
void loadNormalZombieAttackPics(int size);

/**
 * 普通僵尸头掉了
 * @param size
 */
void loadNormalZombieHeadPics(int size);

/**
 * 普通僵尸头掉了后的身体
 * @param size
 */
void loadNormalZombieLostHeadPics(int size);

/**
 * 普通僵尸死亡
 * @param size
 */
void loadNormalZombieDiePics(int size);

/**
 * 普通僵尸因爆炸死亡
 * @param size
 */
void loadZombieBoomDiePics(int size);

void loadSunflowerPics(int index, int size);
void loadPeashooterPics(int index, int size);
void loadPotatoMinePics(int index, int size);
void loadJalapenoPics(int index, int size);
void loadChomperPics(int index, int size);
void loadRepeatPeaShootPics(int index, int size);

#endif //PVZ_M_LOCAL_RESOURCES_H
