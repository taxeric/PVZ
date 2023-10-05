//
// Created by 幻弦让叶 on 2023/10/4.
//

#ifndef PVZ_M_LOCAL_RESOURCES_H
#define PVZ_M_LOCAL_RESOURCES_H

enum {
    SUNFLOWER,
    PEASHOOT,
    POTATOMINE,
    JALAPENO,
    CHOMPER,
    REPEATERPEA,
    PLANTS_COUNT,
};

#define BASE_RES_PATH "../res/graphics/"

#define BASE_RES_START_MENU_PATH "../res/graphics/Screen/MainMenu.png"
#define BASE_RES_ADVENTURE_0_PATH "../res/graphics/Screen/Adventure_0.png"
#define BASE_RES_ADVENTURE_1_PATH "../res/graphics/Screen/Adventure_1.png"

#define BASE_RES_BG_PATH "../res/graphics/Items/Background/Background_0.jpg"
#define BASE_RES_BAR_BG_PATH "../res/graphics/Screen/ChooserBackground.png"

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

void loadSunflowerPics(int size);
void loadPeashooterPics(int size);
void loadPotatoMinePics(int size);
void loadJalapenoPics(int size);
void loadChomperPics(int size);
void loadRepeatPeaShootPics(int size);

#endif //PVZ_M_LOCAL_RESOURCES_H
