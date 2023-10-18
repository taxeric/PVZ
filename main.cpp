#include <iostream>
#include <graphics.h>
#include <mmsystem.h>
#include <conio.h>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <vector>
#include <map>
#include "tools/tools.h"
#include "m_local_resources.h"
#include "models/Land.h"
#include "models/SunshineBall.h"
#include "models/Zombie.h"
#include "models/Bullet.h"
#include "models/GameStatus.h"
#include "models/plants/Sunflower.h"
#include "models/plants/Peashooter.h"
#include "models/plants/RepeaterPea.h"
#include "models/plants/PotatoMine.h"
#include "models/plants/SnowPea.h"

#pragma comment(lib, "winmm.lib")

using namespace std;

#define WIN_WIDTH 900
#define WIN_HEIGHT 600
//游戏界面偏移
#define WIN_OFFSET 130
//图片资源最大数量
#define BASE_RES_PICS_AMOUNT 21

//游戏开始前已选择植物卡槽起点
#define GAME_PLANT_CARD_SLOT_CHOICE_X 80
//游戏开始前植物卡槽仓库起点
#define GAME_PLANT_CARD_SLOT_STORE_X 23

//卡槽起始坐标(包含左侧阳光数)
#define CARD_SLOT_START_X (250 - WIN_OFFSET)
#define CARD_SLOT_START_Y 0

//卡片宽高
#define BASE_CARD_WIDTH 52
#define BASE_CARD_HEIGHT 72
//卡槽之间间距
#define SPACE_BETWEEN_CARD 2
//卡片卡槽起始坐标(不包含左侧阳光数)
#define CARD_START_X (325 - WIN_OFFSET)
#define CARD_START_Y 7

//土地行列数
#define LAND_MAP_ROW 5
#define LAND_MAP_COLUMN 9
//土地左上角起始坐标
#define LAND_MAP_START_X (250 - WIN_OFFSET)
#define LAND_MAP_START_Y 80
//土地右下角结束坐标
#define LAND_MAP_END_X (988 - WIN_OFFSET)
#define LAND_MAP_END_Y 580
//每块土地宽高
#define LAND_MAP_SINGLE_WIDTH 80
#define LAND_MAP_SINGLE_HEIGHT 100

//阳光每次增加的数量
#define SUNSHINE_AMOUNT 25
//阳光总量文字起始坐标
#define SUNSHINE_TEXT_START_X (270 - WIN_OFFSET)
#define SUNSHINE_TEXT_START_Y 57
//阳光飞跃时每次移动的像素 越大越快
#define SUNSHINE_FLY_PIXEL 10

//僵尸站立图片数量
#define AMOUNT_ZOMBIE_STAND_PIC_1 5
//僵尸攻击图片数量
#define AMOUNT_ZOMBIE_ATTACK_PIC_1 21
//僵尸死亡图片数量
#define AMOUNT_ZOMBIE_DEAD_PIC_1 17
#define AMOUNT_ZOMBIE_DEAD_PIC_2 9

//僵尸冻结帧计次(当受到寒冰子弹攻击时开始计次)
#define TIMER_ZOMBIE_FREEZE_FRAME 5

//当前关卡
int game_level;
//游戏状态
struct GameStatus gameStatus[5];

std::map<int, Plant*> globalPlantMap;

//卡槽之间的间距总和
int gross_card_slot_space_x = 0;
//卡槽起点终点x坐标
int card_slot_x_coordinate[PLANTS_COUNT][2];

//当前选中植物在移动过程中的位置
int curMovePlantX, curMovePlantY;
//当前移动的植物位置, 从1开始, 用于判断是否选中植物, 0->未选择植物
int curMovePlantPos;
//当前移动的植物卡槽下标
int curMovePlantCardSlotIndex;

//土地
struct Land landMap[LAND_MAP_ROW][LAND_MAP_COLUMN];
//阳光池
struct SunshineBall sunshineBalls[10];
//阳光图片
IMAGE imgSunshineBallPics[BASE_RES_PICS_AMOUNT];
//阳光总数(废弃, 使用GameStatus[game_level]->sunshine代替)
int gross_sunshine;
//阳光pic宽高
int sunshinePicWidth, sunshinePicHeight;

IMAGE imgBg;
IMAGE imgBar;
IMAGE imgChoosePlants;
//全局植物卡槽图片, 游戏内的植物卡槽图片都通过它获取
IMAGE imgGlobalCardsPics[PLANTS_COUNT];
//全局植物图片, 游戏内的植物动图都通过它获取
IMAGE* imgGlobalPlantsPics[PLANTS_COUNT][BASE_RES_PICS_AMOUNT];

//僵尸池
struct Zombie zombies[10];
//普通僵尸行走图片
IMAGE imgZombiesPics[BASE_RES_PICS_AMOUNT];
//普通僵尸站立图片
IMAGE imgZombiesStandPics[AMOUNT_ZOMBIE_STAND_PIC_1];
//普通僵尸攻击图片
IMAGE imgZombiesAttackPics1[AMOUNT_ZOMBIE_ATTACK_PIC_1];
//僵尸死亡图片
IMAGE imgZombiesDeadPics1[AMOUNT_ZOMBIE_DEAD_PIC_1];
IMAGE imgZombiesDeadPics2[AMOUNT_ZOMBIE_DEAD_PIC_2];
//僵尸冻结计次
int zombieFreezeTimer;

//普通子弹池
struct Bullet normalBullets[30];
//寒冰子弹池
struct Bullet snowBullets[30];
//正常豌豆子弹
IMAGE imgBulletNormal;
//寒冰射手子弹
IMAGE imgBulletSnow;
//豌豆子弹碰撞后
IMAGE imgBulletNormalExplode[4];
//土豆地雷loading
IMAGE imgPotatoMineLoading;
//土豆地雷boom
IMAGE imgPotatoMineExplode;

int getDelay() {
    static unsigned long long lastTime = 0;
    unsigned long long curTime = GetTickCount();
    if (lastTime == 0) {
        lastTime = curTime;
        return 0;
    }
    int ret = curTime - lastTime;
    lastTime = curTime;
    return ret;
}

Plant* generatePlantByType(Plant* choicePlantFromCardSlot, int type) {
    if (choicePlantFromCardSlot == nullptr) {
        return nullptr;
    }
    Plant* plant = nullptr;
    switch (type) {
        case 0: plant = new Sunflower(choicePlantFromCardSlot); break;
        case 1: plant = new Peashooter(choicePlantFromCardSlot); break;
        case 2: plant = new PotatoMine(choicePlantFromCardSlot); break;
        case 3: plant = new SnowPea(choicePlantFromCardSlot); break;
        case 4: plant = new RepeaterPea(choicePlantFromCardSlot); break;
        default:break;
    }
    return plant;
}

void clearPlantPointer(int row, int column) {
    if (row >= LAND_MAP_ROW || row < 0 || column >= LAND_MAP_COLUMN || column < 0) {
        return;
    }
    if (landMap[row][column].plant != nullptr) {
        delete landMap[row][column].plant;
        landMap[row][column].plant = nullptr;
    }
}

void gameInit() {
    loadimage(&imgBg, BASE_RES_BG_PATH);
    loadimage(&imgBar, BASE_RES_BAR_BG_PATH);
    loadimage(&imgChoosePlants, BASE_RES_CHOOSE_PLANTS_PATH);

    globalPlantMap.insert(make_pair(SUNFLOWER, new Sunflower("", "", 0, SUNFLOWER)));
    globalPlantMap.insert(make_pair(PEASHOOT, new Peashooter("", "", 0, PEASHOOT)));
    globalPlantMap.insert(make_pair(POTATOMINE, new PotatoMine(POTATOMINE)));
    globalPlantMap.insert(make_pair(SNOWPEA, new SnowPea(SNOWPEA)));
    globalPlantMap.insert(make_pair(REPEATERPEA, new RepeaterPea("", "", 0, REPEATERPEA)));

    game_level = 0;
    gameStatus[game_level].levelStatus = GameIdle;
    gameStatus[game_level].level = game_level + 1;
    gameStatus[game_level].killCount = 0;
    gameStatus[game_level].zombieMaxCount = 10;
    gameStatus[game_level].sunshine = 900;
    memset(gameStatus[game_level].choosePlantsIndex, -1, sizeof(gameStatus[game_level].choosePlantsIndex));

    memset(sunshineBalls, 0, sizeof(sunshineBalls));
    //加载阳光图片
    loadSunshineBallPics(BASE_RES_PICS_AMOUNT);
    //设置图片宽高
    sunshinePicWidth = imgSunshineBallPics[0].getwidth();
    sunshinePicHeight = imgSunshineBallPics[0].getheight();
    if (sunshinePicWidth <= 0 || sunshinePicHeight <= 0) {
        sunshinePicWidth = 80;
        sunshinePicHeight = 80;
    }
    //随机数种子
    srand(time(nullptr));

    //加载植物卡槽图片
    loadimage(&imgGlobalCardsPics[0], RES_CARD_PIC_SUNFLOWER, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
    loadimage(&imgGlobalCardsPics[1], RES_CARD_PIC_PEASHOOTER, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
    loadimage(&imgGlobalCardsPics[2], RES_CARD_PIC_POTATOMINE, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
//    loadimage(&imgGlobalCardsPics[3], RES_CARD_PIC_JALAPENO, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
//    loadimage(&imgGlobalCardsPics[4], RES_CARD_PIC_CHOMPER, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
    loadimage(&imgGlobalCardsPics[3], RES_CARD_PIC_SNOWPEA, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
    loadimage(&imgGlobalCardsPics[4], RES_CARD_PIC_REPEATERPEA, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);

    //土地
    memset(landMap, 0, sizeof(landMap));

    //加载植物图片
    memset(imgGlobalPlantsPics, 0, sizeof(imgGlobalPlantsPics));
    loadSunflowerPics(0, 17);
    loadPeashooterPics(1, 12);
    loadPotatoMinePics(2, 12);
//    loadJalapenoPics(7);
//    loadChomperPics(12);
    loadSnowPeaPics(3, 14);
    loadRepeatPeaShootPics(4, 14);

    //加载僵尸数据
    memset(zombies, 0, sizeof(zombies));
    loadNormalZombieWalkPics(21);
    loadNormalZombieAttackPics(AMOUNT_ZOMBIE_ATTACK_PIC_1);
    loadNormalZombieStandPics(AMOUNT_ZOMBIE_STAND_PIC_1);
    loadNormalZombieDiePics(AMOUNT_ZOMBIE_DEAD_PIC_2);

    //加载子弹数据
    memset(normalBullets, 0, sizeof(normalBullets));
    loadimage(&imgBulletNormal, RES_PIC_BULLET_PEA_NORMAL);
    memset(snowBullets, 0, sizeof(snowBullets));
    loadimage(&imgBulletSnow, RES_PIC_BULLET_ICE);
    loadimage(&imgBulletNormalExplode[3], RES_PIC_BULLET_PEA_NORMAL_EXPLODE);
    for (int i = 0; i < 3; i ++) {
        float scale = (i + 1) * 0.2;
        loadimage(
                &imgBulletNormalExplode[i],
                RES_PIC_BULLET_PEA_NORMAL_EXPLODE,
                imgBulletNormalExplode[3].getwidth() * scale,
                imgBulletNormalExplode[3].getheight() * scale,
                true
                );
    }

    //杂项
    loadimage(&imgPotatoMineLoading, RES_PIC_POTATOMINE_INIT);
    loadimage(&imgPotatoMineExplode, RES_PIC_POTATOMINE_BOOM);

    initgraph(WIN_WIDTH, WIN_HEIGHT, 1);

    curMovePlantPos = 0;
    curMovePlantCardSlotIndex = -1;
    zombieFreezeTimer = 2;
    gross_sunshine = 50;

//    gross_card_slot_space_x = (PLANTS_COUNT - 1) * SPACE_BETWEEN_CARD;

    LOGFONT f;
    gettextstyle(&f);
    f.lfHeight = 30;
    f.lfWeight = 15;
    strcpy(f.lfFaceName, "Segoe UI Black");
    f.lfQuality = ANTIALIASED_QUALITY;
    settextstyle(&f);
    setbkmode(TRANSPARENT);
    setcolor(BLACK);
}

/**
 * 绘制卡槽
 */
 void drawCardSlot() {
    //绘制卡槽
    int space_x = 0;
    for (int i = 0; i < gameStatus[game_level].choosePlants.size(); i ++) {
        long int x = CARD_START_X + i * BASE_CARD_WIDTH;
        if (i > 0) {
            space_x += SPACE_BETWEEN_CARD;
            x += space_x;
        }
        if (card_slot_x_coordinate[i][0] <= 0 && card_slot_x_coordinate[i][1] <= 0) {
            card_slot_x_coordinate[i][0] = x;
            card_slot_x_coordinate[i][1] = x + BASE_CARD_WIDTH;
        }
        putimage(x, CARD_START_Y, &imgGlobalCardsPics[gameStatus[game_level].choosePlants[i]->index]);
    }
 }

/**
 * 绘制土地植物
 */
void drawPlants() {
    for (int row = 0; row < LAND_MAP_ROW; row ++) {
        for (int column = 0; column < LAND_MAP_COLUMN; column ++) {
            if (landMap[row][column].type > 0) {
                //获取当前选择的植物下标
                int curPlantIndex = landMap[row][column].type - 1;
                IMAGE* img = imgGlobalPlantsPics[curPlantIndex][landMap[row][column].frameIndex];
                int x = LAND_MAP_START_X + column * LAND_MAP_SINGLE_WIDTH + (LAND_MAP_SINGLE_WIDTH - img->getwidth()) / 2;
                int y = LAND_MAP_START_Y + row * LAND_MAP_SINGLE_HEIGHT + (LAND_MAP_SINGLE_HEIGHT - img->getheight()) / 2;
                if (landMap[row][column].type - 1 == POTATOMINE) {
                    PotatoMine* potatoMine = dynamic_cast<PotatoMine*>(landMap[row][column].plant);
                    if (potatoMine->loading) {
                        putimagePng2(x, y, &imgPotatoMineLoading);
                    } else {
                        if (potatoMine->explode) {
                            putimagePng2(x, y, &imgPotatoMineExplode);
                        } else {
                            putimagePng2(x, y, imgGlobalPlantsPics[curPlantIndex][landMap[row][column].frameIndex]);
                        }
                    }
                } else {
                    putimagePng2(x, y, imgGlobalPlantsPics[curPlantIndex][landMap[row][column].frameIndex]);
                }
            }
        }
    }
}

/**
 * 绘制僵尸
 */
void drawZombies() {
    int zombieMax = sizeof(zombies) / sizeof(zombies[0]);
    for (int i = 0; i < zombieMax; i ++) {
        if (zombies[i].isUsed) {
            IMAGE* img;
            if (zombies[i].dead) {
                img = imgZombiesDeadPics2;
            } else {
                if (zombies[i].eating) {
                    img = imgZombiesAttackPics1;
                } else {
                    img = imgZombiesPics;
                }
            }
            img += zombies[i].frameIndex;
            putimagePng2(zombies[i].x, zombies[i].y - img->getheight(), img);
        }
    }
}

/**
 * 绘制子弹
 */
void drawBullets() {
    int normalBulletsMax = sizeof(normalBullets) / sizeof(normalBullets[0]);
    for (int i = 0; i < normalBulletsMax; i ++) {
        if (normalBullets[i].isUsed) {
            if (normalBullets[i].explosion) {
                IMAGE *img = &imgBulletNormalExplode[normalBullets[i].frameIndex];
                putimagePng2(normalBullets[i].x, normalBullets[i].y, img);
            } else {
                putimagePng2(normalBullets[i].x, normalBullets[i].y, &imgBulletNormal);
            }
        }
    }
    int snowBulletsMax = sizeof(snowBullets) / sizeof(snowBullets[0]);
    for (int i = 0; i < normalBulletsMax; i ++) {
        if (snowBullets[i].isUsed) {
            if (snowBullets[i].explosion) {
                IMAGE *img = &imgBulletNormalExplode[snowBullets[i].frameIndex];
                putimagePng2(snowBullets[i].x, snowBullets[i].y, img);
            } else {
                putimagePng2(snowBullets[i].x, snowBullets[i].y, &imgBulletSnow);
            }
        }
    }
}

/**
 * 绘制阳光球
 */
void drawSunshineBalls() {
    int sunshineBallMax = sizeof(sunshineBalls) / sizeof(sunshineBalls[0]);
    for (int i = 0; i < sunshineBallMax; i ++) {
        if (sunshineBalls[i].isUsed || sunshineBalls[i].xOffset > 0) {
            IMAGE* sunshineImg = &imgSunshineBallPics[sunshineBalls[i].frameIndex];
            putimagePng2(sunshineBalls[i].x, sunshineBalls[i].y, sunshineImg);
        }
    }
}

/**
 * 绘制阳光数量
 */
 void drawSunshineScore() {
    char scoreText[8];
    sprintf_s(scoreText, sizeof(scoreText), "%d", gameStatus[game_level].sunshine);
    outtextxy(SUNSHINE_TEXT_START_X, SUNSHINE_TEXT_START_Y, scoreText);
 }

void updateWindow() {
    //缓冲
    BeginBatchDraw();

    putimage(- WIN_OFFSET, 0, &imgBg);
    putimage(CARD_SLOT_START_X, CARD_SLOT_START_Y, &imgBar);
    setbkcolor(TRANSPARENT);

    drawCardSlot();

    //拖动绘制
    if (curMovePlantPos > 0) {
        IMAGE* img = imgGlobalPlantsPics[curMovePlantPos - 1][0];
        putimagePng2(curMovePlantX - img->getwidth() / 2, curMovePlantY - img->getheight() / 2, img);
    }

    drawSunshineScore();

    drawPlants();
    drawZombies();
    drawSunshineBalls();
    drawBullets();

    //结束缓冲
    EndBatchDraw();
}

void collectSunshine(ExMessage* message) {
    int count = sizeof(sunshineBalls) / sizeof(sunshineBalls[0]);
    for (int i = 0; i < count; i ++) {
        struct SunshineBall* sunshineBall = &sunshineBalls[i];
        if (sunshineBall->isUsed) {
            int x = sunshineBall->x;
            int y = sunshineBall->y;
            bool x_value = message->x > x && message->x < x + sunshinePicWidth;
            bool y_value = message->y > y && message->y < y + sunshinePicHeight;
            if (x_value && y_value) {
                sunshineBall->isUsed = false;
                sunshineBall->status = SUNSHINE_COLLECT;
                sunshineBall->speed = 1.0;
//                gross_sunshine += SUNSHINE_AMOUNT;
                gameStatus[game_level].sunshine += SUNSHINE_AMOUNT;
                playSounds(SOUND_COLLECT_POINT);
                //设置偏移
                float destX = CARD_SLOT_START_X;
                float destY = CARD_SLOT_START_Y;
                float angle = atan(((float)y - destY) / ((float)x - destX));
                sunshineBalls[i].xOffset = SUNSHINE_FLY_PIXEL * cos(angle);
                sunshineBalls[i].yOffset = SUNSHINE_FLY_PIXEL * sin(angle);
                break;
            }
        }
    }
}

void userClickEvent() {
    ExMessage message{};
    static int status = 0;
    int choosePlantsCount = gameStatus[game_level].choosePlants.size();
    if (peekmessage(&message)) {
        if (message.message == WM_LBUTTONDOWN) {
            //鼠标按下事件
            //植物卡槽x范围
            bool x_value = message.x > CARD_START_X && message.x < CARD_START_X + BASE_CARD_WIDTH * choosePlantsCount + gross_card_slot_space_x;
            //植物卡槽y范围
            bool y_value = message.y > CARD_START_Y && message.y < BASE_CARD_HEIGHT;
            if (x_value && y_value) {
                for (int x_index = 0; x_index < gameStatus[game_level].choosePlants.size(); x_index ++) {
                    //当前点击的植物
                    Plant* plant = gameStatus[game_level].choosePlants[x_index];
                    //当前阳光
                    int sunshine = gameStatus[game_level].sunshine;
                    //检查是否点击了占位
                    if (message.x > card_slot_x_coordinate[x_index][0] && message.x < card_slot_x_coordinate[x_index][1]) {
                        //检查阳光
                        if (sunshine >= plant->sunshine) {
                            status = 1;
                            curMovePlantX = message.x;
                            curMovePlantY = message.y;
                            curMovePlantPos = plant->index + 1;
                            curMovePlantCardSlotIndex = x_index;
                            if (getcolor() != BLACK) {
                                setcolor(BLACK);
                                drawSunshineScore();
                            }
                        } else {
                            setcolor(RED);
                        }
                        break;
                    }
                }
            } else {
                //收集阳光
                collectSunshine(&message);
            }
        } else if (message.message == WM_MOUSEMOVE && status == 1) {
            //鼠标移动事件
            curMovePlantX = message.x;
            curMovePlantY = message.y;
        } else if (message.message == WM_LBUTTONUP) {
            //鼠标抬起事件
            if (status == 1) {
                //x范围
                int x_value = message.x > LAND_MAP_START_X && message.x < LAND_MAP_END_X;
                //y范围
                int y_value = message.y > LAND_MAP_START_Y && message.y < LAND_MAP_END_Y;
                if (x_value && y_value) {
                    int row = (message.y - LAND_MAP_START_Y) / LAND_MAP_SINGLE_HEIGHT;
                    int column = (message.x - LAND_MAP_START_X) / LAND_MAP_SINGLE_WIDTH;
                    struct Land* land = &landMap[row][column];
                    if (land->type <= 0) {
                        land->type = curMovePlantPos;
                        land->frameIndex = 0;
                        land->caught = false;
                        land->x = LAND_MAP_START_X + column * LAND_MAP_SINGLE_WIDTH;
                        land->y = LAND_MAP_START_Y + row * LAND_MAP_SINGLE_HEIGHT;
                        land->hp = gameStatus[game_level].choosePlants[curMovePlantCardSlotIndex]->hp;
                        land->plant = generatePlantByType(
                                gameStatus[game_level].choosePlants[curMovePlantCardSlotIndex],
                                curMovePlantPos - 1
                                );
                        if (land->plant != nullptr) {
                            land->plant->row = row;
                            land->plant->column = column;
                        }
                        gameStatus[game_level].sunshine -= gameStatus[game_level].choosePlants[curMovePlantCardSlotIndex]->sunshine;
                        cout << "event: [plant] (x,y) -> (" << land->plant->row << "," << land->plant->column << "); plant index = " << land->type
                        << "; need sunshine = " << gameStatus[game_level].choosePlants[curMovePlantCardSlotIndex]->sunshine
                        << "; current sunshine = " << gameStatus[game_level].sunshine
                        << endl;
                        int rm = rand() % 2;
                        playSounds(rm == 0 ? SOUND_PLANT_1 : SOUND_PLANT_2);
                    }
                }
                status = 0;
                curMovePlantPos = 0;
                curMovePlantCardSlotIndex = -1;
            }
        }
    }
}

void updatePlants() {
    static int count = 0;
    count ++;
    if (count > 3) {
        count = 0;
        for (int i = 0; i < LAND_MAP_ROW; i++) {
            for (int j = 0; j < LAND_MAP_COLUMN; j++) {
                if (landMap[i][j].type > 0) {
                    landMap[i][j].frameIndex++;
                    int plantIndex = landMap[i][j].type - 1;
                    int frameIndex = landMap[i][j].frameIndex;
                    if (imgGlobalPlantsPics[plantIndex][frameIndex] == nullptr) {
                        landMap[i][j].frameIndex = 0;
                    }
                }
            }
        }
    }
}

void createSunshine() {
    static int count = 0;
    static int fre = 80;//自然掉落的阳光生成间隔
    count ++;
    if (count >= fre) {
        fre = 600 + rand() % 20;
        count = 0;
        int ballMax = sizeof(sunshineBalls) / sizeof(sunshineBalls[0]);
        int i;
        for (i = 0; i < ballMax && sunshineBalls[i].isUsed; i ++);
        if (i >= ballMax) {
            return;
        }
        struct SunshineBall* sunshineBall = &sunshineBalls[i];
        sunshineBall->isUsed = true;
        sunshineBall->frameIndex = 0;
        sunshineBall->x = LAND_MAP_START_X + rand() % (LAND_MAP_END_X - LAND_MAP_START_X);
        sunshineBall->y = LAND_MAP_START_Y;
        sunshineBall->destY = LAND_MAP_START_Y + (rand() % 4) * 90;
        sunshineBall->timer = 0;
        sunshineBall->xOffset = 0;
        sunshineBall->yOffset = 0;
        sunshineBall->status = SUNSHINE_DOWN;
    }

    int sunshineBallMax = sizeof(sunshineBalls) / sizeof(sunshineBalls[0]);
    for (int row = 0; row < LAND_MAP_ROW; row ++) {
        for (int column = 0; column < LAND_MAP_COLUMN; column ++) {
            if (landMap[row][column].type - 1 == SUNFLOWER) {
                landMap[row][column].timer ++;
                if (landMap[row][column].timer > 600) {//向日葵生成的阳光间隔
                    landMap[row][column].timer = 0;
                    int k;
                    for (k = 0; k < sunshineBallMax && sunshineBalls[k].isUsed; k ++);
                    if (k >= sunshineBallMax) {
                        return;
                    }
                    IMAGE* sunflowerImg = imgGlobalPlantsPics[SUNFLOWER][0];
                    struct SunshineBall* sunshineBall = &sunshineBalls[k];
                    sunshineBall->isUsed = true;
                    sunshineBall->x = landMap[row][column].x;
                    sunshineBall->y = landMap[row][column].y;
                    sunshineBall->destX = landMap[row][column].x + sunflowerImg->getwidth() / 2;
                    sunshineBall->destY = landMap[row][column].y + imgSunshineBallPics[0].getheight() / 2;
                    sunshineBall->status = SUNSHINE_PRODUCT;
                    sunshineBall->speed = 0.05;
                }
            }
        }
    }
}

void updateSunshine() {
    int ballMax = sizeof(sunshineBalls) / sizeof(sunshineBalls[0]);
    for (int i = 0; i < ballMax; i ++) {
        struct SunshineBall* sunshineBall = &sunshineBalls[i];
        if (sunshineBalls[i].isUsed) {
            sunshineBall->frameIndex = (sunshineBalls[i].frameIndex + 1) % BASE_RES_PICS_AMOUNT;
            int status = sunshineBall->status;
            if (status == SUNSHINE_DOWN) {
                if (sunshineBall->y >= sunshineBall->destY) {
                    sunshineBall->timer = 0;
                    sunshineBall->status = SUNSHINE_GROUND;
                } else {
                    sunshineBall->y += 2;
                }
            } else if (status == SUNSHINE_PRODUCT) {
                if (sunshineBall->x >= sunshineBall->destX) {
                    if (sunshineBall->y >= sunshineBall->destY) {
                        sunshineBall->timer = 0;
                        sunshineBall->status = SUNSHINE_GROUND;
                    } else {
                        sunshineBall->y += 2;
                    }
                } else {
                    sunshineBall->x ++;
                }
            } else if (status == SUNSHINE_GROUND) {
                sunshineBall->timer ++;
                if (sunshineBall->timer > 100) {
                    sunshineBall->isUsed = false;
                    sunshineBall->timer = 0;
                }
            }
        } else if (sunshineBall->xOffset > 0) {
            //设置偏移
            float destX = CARD_SLOT_START_X;
            float destY = CARD_SLOT_START_Y;
            float angle = atan(((float) sunshineBall->y - destY) / ((float) sunshineBall->x - destX));
            sunshineBall->xOffset = SUNSHINE_FLY_PIXEL * cos(angle);
            sunshineBall->yOffset = SUNSHINE_FLY_PIXEL * sin(angle);

            sunshineBall->x -= sunshineBall->xOffset;
            sunshineBall->y -= sunshineBall->yOffset;
            if (sunshineBall->x <= CARD_SLOT_START_X || sunshineBall->y <= CARD_SLOT_START_Y) {
                sunshineBall->xOffset = 0;
                sunshineBall->yOffset = 0;
            }
        }
    }
}

void createZombies() {
    if (gameStatus[game_level].zombieCount >= gameStatus[game_level].zombieMaxCount) {
        return;
    }

    static int zombieFre = 40;//僵尸生成间隔
    static int count = 0;
    count ++;
    if (count > zombieFre) {
        count = 0;
        zombieFre = rand() % 200 + 600;
        int i;
        int zombieMax = sizeof(zombies) / sizeof(zombies[0]);
        for (i = 0; i < zombieMax && zombies[i].isUsed; i ++);
        if (i < zombieMax) {
            struct Zombie* zombie = &zombies[i];
            memset(&zombies[i], 0, sizeof(zombies[i]));
            zombie->isUsed = true;
            zombie->frameIndex = 0;
            zombie->x = WIN_WIDTH;
            zombie->row = rand() % LAND_MAP_ROW;
            zombie->y = LAND_MAP_START_Y * 2 + (zombie->row) * LAND_MAP_SINGLE_HEIGHT;
            zombie->speed = 1;
            zombie->hp = 100;
            zombie->head = false;
            zombie->lostHead = false;
            zombie->dead = false;
            zombie->attackRow = -1;
            zombie->attackColumn = -1;
            zombie->freeze = false;
            zombie->freezeTimer = 8;
            gameStatus[game_level].zombieCount ++;
            if (gameStatus[game_level].levelStatus == GameIdle) {
                gameStatus[game_level].levelStatus = GameRunning;
                playSounds(SOUND_ZOMBIES_ARE_COMING);
            }
        }
    }
}

void updateZombies() {
    int zombieMax = sizeof(zombies) / sizeof(zombies[0]);
    static int count = 0;
    count ++;
    if (count > 4) {
        count = 0;
        for (int i = 0; i < zombieMax; i ++) {
            if (zombies[i].isUsed) {
                if (!zombies[i].eating) {
                    if ((zombies[i].frameIndex > 0 && zombies[i].frameIndex < 14) ||
                        (zombies[i].frameIndex > 16 && zombies[i].frameIndex < BASE_RES_PICS_AMOUNT - 2)) {
                        static int freezeCount = 0;
                        if (zombies[i].freeze) {
                            freezeCount ++;
                            if (freezeCount > 4) {
                                freezeCount = 0;
                                zombies[i].x -= zombies[i].speed;
                            }
                        } else {
                            zombies[i].x -= zombies[i].speed;
                        }
                    }
                }
                if (zombies[i].x < LAND_MAP_START_X - 80) {//僵尸进入房子了🧠
                    //game over ~~~
                    cout << "game over ~~~" << endl;
                    gameStatus[game_level].levelStatus = GameFailed;
                }
            }
        }
    }

    static int zombieActionCount = 0;
    zombieActionCount ++;
    if (zombieActionCount > 4 * 2) {
        zombieActionCount = 0;
        for (int i = 0; i < zombieMax; i ++) {
            if (zombies[i].isUsed) {
                if (zombies[i].dead) {
                    zombies[i].frameIndex ++;
                    if (zombies[i].frameIndex >= AMOUNT_ZOMBIE_DEAD_PIC_2) {
                        zombies[i].isUsed = false;
                        gameStatus[game_level].killCount ++;
                        if (gameStatus[game_level].killCount == gameStatus[game_level].zombieMaxCount) {
                            gameStatus[game_level].levelStatus = GameSuccess;
                        }
                    }
                } else {
                    static int freezeActionCount = 0;
                    if (zombies[i].freeze && zombies[i].freezeTimer > 0) {
                        freezeActionCount ++;
                        zombies[i].freezeTimer -= 1;
                        if (zombies[i].freezeTimer <= 0) {
                            zombies[i].freezeTimer = 8;
                            zombies[i].freeze = false;
                        } else {
                            if (freezeActionCount > 4) {
                                freezeActionCount = 0;
                                playSounds(SOUND_FROZEN);
                                if (zombies[i].eating) {
                                    zombies[i].frameIndex = (zombies[i].frameIndex + 1) % AMOUNT_ZOMBIE_ATTACK_PIC_1;
                                } else {
                                    zombies[i].frameIndex = (zombies[i].frameIndex + 1) % BASE_RES_PICS_AMOUNT;
                                }
                            }
                        }
                    } else {
                        if (zombies[i].eating) {
                            zombies[i].frameIndex = (zombies[i].frameIndex + 1) % AMOUNT_ZOMBIE_ATTACK_PIC_1;
                        } else {
                            zombies[i].frameIndex = (zombies[i].frameIndex + 1) % BASE_RES_PICS_AMOUNT;
                        }
                    }
                }
            }
        }
    }
}

void plantsShoot() {
    int lines[LAND_MAP_ROW] = {0};
    int zombieCount = sizeof(zombies) / sizeof(zombies[0]);
    int dangerX = LAND_MAP_END_X - 80/* - imgZombiesPics[0].getwidth()*/;//手动减去僵尸png前方的占位透明像素
    int normalBulletMax = sizeof(normalBullets) / sizeof(normalBullets[0]);
    int snowBulletMax = sizeof(snowBullets) / sizeof(snowBullets[0]);
    for (int i = 0; i < zombieCount; i ++) {
        if (zombies[i].isUsed && zombies[i].x < dangerX) {
            lines[zombies[i].row] = 1;
        }
    }

    for (int row = 0; row < LAND_MAP_ROW; row ++) {
        for (int column = 0; column < LAND_MAP_COLUMN; column ++) {
            if (landMap[row][column].type == 0) {
                continue;
            }
            if (landMap[row][column].type - 1 == PEASHOOT && lines[row]) {
                static int count = 0;
                count ++;
                if (count > 120) {//越大子弹间隔越大
                    count = 0;
                    int k;
                    //找到可用的子弹
                    for (k = 0; k < normalBulletMax && normalBullets[k].isUsed; k ++);
                    if (k < normalBulletMax) {
                        normalBullets[k].isUsed = true;
                        normalBullets[k].row = row;
                        normalBullets[k].speed = 4;

                        normalBullets[k].explosion = false;
                        normalBullets[k].frameIndex = 0;

                        int plantX = LAND_MAP_START_X + column * LAND_MAP_SINGLE_WIDTH;
                        int plantY = LAND_MAP_START_Y + row * LAND_MAP_SINGLE_HEIGHT;
                        normalBullets[k].x = plantX + imgGlobalPlantsPics[landMap[row][column].type - 1][0]->getwidth() - 10;
                        normalBullets[k].y = plantY + 5;
                    }
                }
            }
            if (landMap[row][column].type - 1 == SNOWPEA && lines[row]) {
                static int count = 0;
                count ++;
                if (count > 120) {
                    count = 0;
                    int k;
                    //找到可用的子弹
                    for (k = 0; k < normalBulletMax && snowBullets[k].isUsed; k ++);
                    if (k < normalBulletMax) {
                        snowBullets[k].isUsed = true;
                        snowBullets[k].row = row;
                        snowBullets[k].speed = 4;

                        snowBullets[k].explosion = false;
                        snowBullets[k].frameIndex = 0;

                        int plantX = LAND_MAP_START_X + column * LAND_MAP_SINGLE_WIDTH;
                        int plantY = LAND_MAP_START_Y + row * LAND_MAP_SINGLE_HEIGHT;
                        snowBullets[k].x = plantX + imgGlobalPlantsPics[landMap[row][column].type - 1][0]->getwidth() - 10;
                        snowBullets[k].y = plantY + 5;
                    }
                }
            }
        }
    }
}

void updateBullets() {
    int normalCountMax = sizeof(normalBullets) / sizeof(normalBullets[0]);
    int snowCountMax = sizeof(snowBullets) / sizeof(snowBullets[0]);
    for (int i = 0; i < normalCountMax; i ++) {
        if (normalBullets[i].isUsed) {
            normalBullets[i].x = normalBullets[i].x + normalBullets[i].speed;
            if (normalBullets[i].x > LAND_MAP_END_X) {
                normalBullets[i].isUsed = false;
            }
            if (normalBullets[i].explosion) {
                normalBullets[i].frameIndex ++;
                if (normalBullets[i].frameIndex >= 4) {
                    normalBullets[i].isUsed = false;
                }
            }
        }
    }
    for (int i = 0; i < snowCountMax; i ++) {
        if (snowBullets[i].isUsed) {
            snowBullets[i].x = snowBullets[i].x + snowBullets[i].speed;
            if (snowBullets[i].x > LAND_MAP_END_X) {
                snowBullets[i].isUsed = false;
            }
            if (snowBullets[i].explosion) {
                snowBullets[i].frameIndex ++;
                if (snowBullets[i].frameIndex >= 4) {
                    snowBullets[i].isUsed = false;
                }
            }
        }
    }
}

void checkBullet2Zombie() {
    int zombieCount = sizeof(zombies) / sizeof(zombies[0]);

    int bulletNormalCount = sizeof(normalBullets) / sizeof(normalBullets[0]);
    for (int i = 0; i < bulletNormalCount; i ++) {
        if (!normalBullets[i].isUsed || normalBullets[i].explosion) {
            continue;
        }
        for (int k = 0; k < zombieCount; k ++) {
            if (!zombies[k].isUsed) {
                continue;
            }
            int zombieX1 = zombies[k].x + 80;//僵尸图片实际需要碰撞的位置起点x, 因为图片尺寸需要手动加上偏移
            int zombieX2 = zombies[k].x + 110;//僵尸图片实际需要碰撞的位置终点x, 因为图片尺寸需要手动加上偏移
            int bulletX = normalBullets[i].x;
            if (!zombies[k].dead && bulletX >= zombieX1 && bulletX <= zombieX2 && normalBullets[i].row == zombies[k].row) {
                zombies[k].hp -= 10;//默认伤害
                normalBullets[i].explosion = true;
                normalBullets[i].speed = 0;
                playSounds(SOUND_PLANT_SPLAT);
                if (zombies[k].hp <= 40 && zombies[k].hp > 0) {
                    zombies[k].lostHead = true;
                } else if (zombies[k].hp <= 0) {
                    zombies[k].dead = true;
                    zombies[k].speed = 0;
                    zombies[k].frameIndex = 0;
                }
                break;
            }
        }
    }

    int bulletSnowCount = sizeof(snowBullets) / sizeof(snowBullets[0]);
    for (int i = 0; i < bulletNormalCount; i ++) {
        if (!snowBullets[i].isUsed || snowBullets[i].explosion) {
            continue;
        }
        for (int k = 0; k < zombieCount; k ++) {
            if (!zombies[k].isUsed) {
                continue;
            }
            int zombieX1 = zombies[k].x + 80;//僵尸图片实际需要碰撞的位置起点x, 因为图片尺寸需要手动加上偏移
            int zombieX2 = zombies[k].x + 110;//僵尸图片实际需要碰撞的位置终点x, 因为图片尺寸需要手动加上偏移
            int bulletX = snowBullets[i].x;
            if (!zombies[k].dead && bulletX >= zombieX1 && bulletX <= zombieX2 && snowBullets[i].row == zombies[k].row) {
                zombies[k].hp -= 10;//默认伤害
                zombies[k].freeze = true;
                zombies[k].freezeTimer = 8;
                snowBullets[i].explosion = true;
                snowBullets[i].speed = 0;
                if (zombies[k].hp <= 40 && zombies[k].hp > 0) {
                    zombies[k].lostHead = true;
                } else if (zombies[k].hp <= 0) {
                    zombies[k].dead = true;
                    zombies[k].speed = 0;
                    zombies[k].frameIndex = 0;
                }
                break;
            }
        }
    }
}

void checkZombie2Plant() {
    int zombieCount = sizeof(zombies) / sizeof(zombies[0]);
    for (int i = 0; i < zombieCount; i ++) {
        if (!zombies[i].dead) {
            int row = zombies[i].row;
            for (int column = 0; column < LAND_MAP_COLUMN; column++) {
                if (landMap[row][column].type > 0) {
                    //植物右侧像素值
                    int plantX1 = LAND_MAP_START_X + column * LAND_MAP_SINGLE_WIDTH + 10;
                    int plantX2 = LAND_MAP_START_X + column * LAND_MAP_SINGLE_WIDTH + LAND_MAP_SINGLE_WIDTH;
                    int zombieX = zombies[i].x + LAND_MAP_SINGLE_WIDTH;
                    if (zombieX > plantX1 && zombieX < plantX2) {
                        static int count = 0;
                        count ++;
                        if (landMap[row][column].caught) {
                            if (!zombies[i].eating) {//如果之后的僵尸没有停下来
                                zombies[i].eating = true;
                                zombies[i].speed = 0;
                                zombies[i].frameIndex = 0;
                                zombies[i].attackRow = row;
                                zombies[i].attackColumn = column;
                            }
                            if (count > 20) {//越大切换图片越慢
                                count = 0;
                                zombies[i].frameIndex ++;
                                landMap[row][column].hp --;
                            }
                            if (landMap[row][column].hp <= 0) {
                                for (int m = 0; m < zombieCount; m ++) {
                                    if (zombies[m].attackRow == row && zombies[m].attackColumn == column) {
                                        zombies[m].attackRow = -1;
                                        zombies[m].attackColumn = -1;
                                        zombies[m].eating = false;
                                        zombies[m].speed = 1;
                                        zombies[m].frameIndex = rand() % BASE_RES_PICS_AMOUNT;
                                    }
                                }
                                playSounds(SOUND_GULP);
                                landMap[row][column].hp = 0;
                                landMap[row][column].caught = false;
                                landMap[row][column].type = 0;
                                clearPlantPointer(row, column);
                            } else {
                                if (zombies[i].frameIndex >= AMOUNT_ZOMBIE_ATTACK_PIC_1) {
                                    zombies[i].frameIndex = 0;
                                }
                            }
                        } else {
                            //在植物种下时已经初始化deadTime=0, 故这里只需重置捕获状态即可
                            landMap[row][column].caught = true;
                            zombies[i].attackRow = row;
                            zombies[i].attackColumn = column;
                            zombies[i].eating = true;
                            zombies[i].speed = 0;
                            zombies[i].frameIndex = 0;
                        }
                    }
                }
            }
        }
    }
}

void potatoMineBoom() {
    int zombieMax = sizeof(zombies) / sizeof(zombies[0]);
    for (int row = 0; row < LAND_MAP_ROW; row ++) {
        for (int column = 0; column < LAND_MAP_COLUMN; column++) {
            if (landMap[row][column].type - 1 == POTATOMINE) {
                auto* potatoMine = dynamic_cast<PotatoMine*>(landMap[row][column].plant);
                if (potatoMine->loading) {
                    potatoMine->loadTimer ++;
                    if (potatoMine->loadTimer >= 1000) {
                        potatoMine->loading = false;//装填完毕
                        landMap[row][column].frameIndex = 0;
                    }
                } else {
                    for (int i = 0; i < zombieMax; i ++) {
                        if (zombies[i].isUsed && zombies[i].row == row) {
                            //植物所占像素值范围
                            int plantX1 = LAND_MAP_START_X + column * LAND_MAP_SINGLE_WIDTH + 10;
                            int plantX2 = LAND_MAP_START_X + column * LAND_MAP_SINGLE_WIDTH + LAND_MAP_SINGLE_WIDTH;
                            int zombieX = zombies[i].x + 80;//僵尸图片实际需要碰撞的位置起点x, 因为图片尺寸需要手动加上偏移
                            if (zombieX >= plantX1 && zombieX <= plantX2) {
                                potatoMine->explode = true;
                                zombies[i].dead = true;
                                zombies[i].hp = 0;
                                zombies[i].speed = 0;
                                zombies[i].frameIndex = 0;
                                playSounds(SOUND_POTATO_BOOM);
                                if (potatoMine->explodeTimer < 10) {
                                    potatoMine->explodeTimer ++;
                                } else {
                                    landMap[row][column].type = 0;
                                    landMap[row][column].hp = 0;
                                    clearPlantPointer(row, column);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void collisionCheck() {
    //子弹碰撞僵尸
    checkBullet2Zombie();
    //僵尸攻击植物
    checkZombie2Plant();
    //土豆地雷explosion
    potatoMineBoom();
}

void updateGame() {

    updatePlants();

    createSunshine();
    updateSunshine();

    createZombies();
    updateZombies();

    plantsShoot();
    updateBullets();

    collisionCheck();
}

void startMenuUI() {
    IMAGE imgStartUIBg, imgAdventure0, imgAdventure1;
    loadimage(&imgStartUIBg, BASE_RES_START_MENU_PATH);
    loadimage(&imgAdventure0, BASE_RES_ADVENTURE_0_PATH, 300, 80);
    loadimage(&imgAdventure1, BASE_RES_ADVENTURE_1_PATH, 300, 80);
    bool action_flag = false;
    bool move_flag = false;
    while (true) {
        BeginBatchDraw();

        putimage(0, 0, &imgStartUIBg);
        putimagePng3(480, 80, move_flag ? &imgAdventure1 : &imgAdventure0);

        ExMessage message{};
        if (peekmessage(&message)) {
            if (message.message == WM_LBUTTONUP && action_flag) {
                EndBatchDraw();
                break;
            } else if (message.message == WM_MOUSEMOVE) {
                bool x_value = message.x > 480 && message.x < 780;
                bool y_value = message.y > 80 && message.y < 160;
                if (x_value && y_value) {
                    move_flag = true;
                    action_flag = true;
                } else {
                    move_flag = false;
                    action_flag = false;
                }
            }
        }

        EndBatchDraw();
    }
}

void evilLaugh() {
    playSounds(SOUND_EVIL_LAUGH);
    Sleep(3000);
}

void viewScene() {
    int xMin = WIN_WIDTH - imgBg.getwidth();
    int zombiesStandCoordinate[9][2] = {0 };
    for (int x = 0; x < 9; x ++) {
        double r = 1.0 * rand() / RAND_MAX;
        int rx = (int) (r * (800 - 600) + 500);
        int ry = (int) (rand() % 400);
        zombiesStandCoordinate[x][0] = rx;
        zombiesStandCoordinate[x][1] = ry;
    }
    for (int x = 0; x >= xMin; x -= 2) {
        BeginBatchDraw();
        putimage(x, 0, &imgBg);
        for (int k = 0; k < 9; k ++) {
            putimagePng2(
                    zombiesStandCoordinate[k][0] - xMin + x,
                    zombiesStandCoordinate[k][1],
                    &imgZombiesStandPics[0]
                    );
        }
        EndBatchDraw();
        Sleep(5);
    }

    int imgBarHeight = imgBar.getheight();
    int startBtnY1 = imgBarHeight + 460;
    int startBtnY2 = startBtnY1 + 30;
    IMAGE startBtn;
    loadimage(&startBtn, BASE_RES_CHOOSE_PLANTS_START_BTN_PATH);
    bool startBtnFlag = false;
    bool choosePlantFlag = false;
    bool removePlantFlag = false;
    //卡槽仓库起点y
    int cardSlotStorePlantY = imgBarHeight + 40;
    while (true) {
        BeginBatchDraw();
        putimage(xMin, 0, &imgBg);
        putimage(0, 0, &imgBar);
        putimage(0, imgBarHeight, &imgChoosePlants);
        if (startBtnFlag) {
            putimage(155, startBtnY1, &startBtn);
        }
        //资源有限, 就手动存5个了先2333~~~
        putimage(GAME_PLANT_CARD_SLOT_STORE_X, cardSlotStorePlantY, &imgGlobalCardsPics[0]);
        putimage(GAME_PLANT_CARD_SLOT_STORE_X + BASE_CARD_WIDTH, cardSlotStorePlantY, &imgGlobalCardsPics[1]);
        putimage(GAME_PLANT_CARD_SLOT_STORE_X + BASE_CARD_WIDTH * 2, cardSlotStorePlantY, &imgGlobalCardsPics[2]);
        putimage(GAME_PLANT_CARD_SLOT_STORE_X + BASE_CARD_WIDTH * 3, cardSlotStorePlantY, &imgGlobalCardsPics[3]);
        putimage(GAME_PLANT_CARD_SLOT_STORE_X + BASE_CARD_WIDTH * 4, cardSlotStorePlantY, &imgGlobalCardsPics[4]);
        for (int k = 0; k < 9; k ++) {
//            int frameIndex = rand() % AMOUNT_ZOMBIE_STAND_PIC_1;
            putimagePng2(
                    zombiesStandCoordinate[k][0],
                    zombiesStandCoordinate[k][1],
                    &imgZombiesStandPics[0]
            );
        }

        for (int i = 0; i < gameStatus[game_level].choosePlants.size(); i ++) {
            Plant* plant = gameStatus[game_level].choosePlants[i];
            putimage(GAME_PLANT_CARD_SLOT_CHOICE_X + BASE_CARD_WIDTH * i, CARD_START_Y, &imgGlobalCardsPics[plant->index]);
        }

        ExMessage msg{};
        if (peekmessage(&msg)) {
            if (msg.message == WM_LBUTTONDOWN) {
                //下面的卡槽仓库点击坐标
                bool x_c_p_1 = msg.x > GAME_PLANT_CARD_SLOT_STORE_X && msg.x < GAME_PLANT_CARD_SLOT_STORE_X + BASE_CARD_WIDTH * 5;
                bool y_c_p_1 = msg.y > cardSlotStorePlantY && msg.y < cardSlotStorePlantY + BASE_CARD_HEIGHT;
                //上面的已选择的植物卡槽点击坐标
                bool x_c_p_2 = msg.x > GAME_PLANT_CARD_SLOT_CHOICE_X && msg.x < GAME_PLANT_CARD_SLOT_CHOICE_X + BASE_CARD_WIDTH * gameStatus[game_level].choosePlants.size();
                bool y_c_p_2 = msg.y > CARD_START_Y && msg.y < CARD_START_Y + BASE_CARD_HEIGHT;
                if (x_c_p_1 && y_c_p_1) {
                    choosePlantFlag = true;
                } else {
                    choosePlantFlag = false;
                }
                if (x_c_p_2 && y_c_p_2) {
                    removePlantFlag = true;
                } else {
                    removePlantFlag = false;
                }
            } else if (msg.message == WM_MOUSEMOVE) {
                bool x_value_start_btn = msg.x > 155 && msg.x < 310;
                bool y_value_start_btn = msg.y > startBtnY1 && msg.y < startBtnY2;
                if (x_value_start_btn && y_value_start_btn) {
                    startBtnFlag = true;
                } else {
                    startBtnFlag = false;
                }
            } else if (msg.message == WM_LBUTTONUP) {
                if (removePlantFlag) {
                    if (!gameStatus[game_level].choosePlants.empty()) {
                        //获取对应已选择的植物卡槽的下标
                        int x_index = (msg.x - GAME_PLANT_CARD_SLOT_CHOICE_X) / BASE_CARD_WIDTH;
                        //移除
                        gameStatus[game_level].choosePlants.erase(
                                gameStatus[game_level].choosePlants.begin() + x_index);
                        cout << "event: [remove plant] (" << gameStatus[game_level].choosePlants.size()
                             << ") plant index = " << x_index << endl;
                    }
                    removePlantFlag = false;
                }
                if (choosePlantFlag) {
                    int x_index = (msg.x - GAME_PLANT_CARD_SLOT_STORE_X) / BASE_CARD_WIDTH;
                    //判断是否已经存在
                    auto plantIte = globalPlantMap.find(x_index);
                    bool isExist = false;
                    for (int e = 0; e < gameStatus[game_level].choosePlants.size(); e ++) {
                        if (gameStatus[game_level].choosePlants[e]->index == x_index) {
                            isExist = true;
                            break;
                        }
                    }
                    //如果植物还没有被选择
                    if (!isExist) {
                        //从全局map中获取对应植物
                        if (plantIte != globalPlantMap.end()) {
                            gameStatus[game_level].choosePlants.push_back(plantIte->second);
                            cout << "event: [choose plant] (" << gameStatus[game_level].choosePlants.size() << ") plant index = " << plantIte->second->index << endl;
                            playSounds(SOUND_FLOOP);
                        }
                    }
                    choosePlantFlag = false;
                }
                if (startBtnFlag) {
                    //等待一段时间
                    for (int i = 0; i < 100; i++) {
                        BeginBatchDraw();
                        putimage(xMin, 0, &imgBg);
                        for (int k = 0; k < 9; k++) {
                            putimagePng2(
                                    zombiesStandCoordinate[k][0],
                                    zombiesStandCoordinate[k][1],
                                    &imgZombiesStandPics[0]
                            );
                        }
                        EndBatchDraw();
                        Sleep(1);
                    }
                    //卡槽总空格
                    gross_card_slot_space_x = (gameStatus[game_level].choosePlants.size() - 1) * SPACE_BETWEEN_CARD;
                    //移动到主屏幕
                    int count = 0;
                    for (int x = xMin; x <= -WIN_OFFSET; x += 1) {
                        BeginBatchDraw();
                        putimage(x, 0, &imgBg);
                        count++;
                        for (int k = 0; k < 9; k++) {
                            putimagePng2(
                                    zombiesStandCoordinate[k][0] - xMin + x,
                                    zombiesStandCoordinate[k][1],
                                    &imgZombiesStandPics[0]
                            );
                            if (count >= 10) {
                                count = 0;
                            }
                        }
                        EndBatchDraw();
                    }
                    EndBatchDraw();
                    break;
                }
            }
        }

        EndBatchDraw();
    }
}

void plantSlotDown() {
    for (int y = -(CARD_SLOT_START_Y + BASE_CARD_HEIGHT); y <= CARD_SLOT_START_Y; y += 2) {
        BeginBatchDraw();
        putimage(- WIN_OFFSET, 0, &imgBg);
        putimage(CARD_SLOT_START_X, y, &imgBar);

        //绘制卡槽
        int space_x = 0;
        for (int i = 0; i < gameStatus[game_level].choosePlants.size(); i ++) {
            long int x = CARD_START_X + i * BASE_CARD_WIDTH;
            if (i > 0) {
                space_x += SPACE_BETWEEN_CARD;
                x += space_x;
            }
            if (card_slot_x_coordinate[i][0] <= 0 && card_slot_x_coordinate[i][1] <= 0) {
                card_slot_x_coordinate[i][0] = x;
                card_slot_x_coordinate[i][1] = x + BASE_CARD_WIDTH;
            }
            putimage(x, y, &imgGlobalCardsPics[gameStatus[game_level].choosePlants[i]->index]);
        }

        EndBatchDraw();
        Sleep(10);
    }
}

bool checkGameStatus() {
    int ret = false;
    int status = gameStatus[game_level].levelStatus;
    if (status == GameSuccess) {
        Sleep(2000);
        //TODO 进入下一关
        ret = true;
    } else if (status == GameFailed) {
        Sleep(2000);
        ret = false;
    }
    return ret;
}

int main() {
    std::cout << "Hello, PVZ!" << std::endl;

    gameInit();
    startMenuUI();
    evilLaugh();

    viewScene();

    plantSlotDown();

    int timer = 0;
    bool refreshFlag = true;
    while (true) {

        userClickEvent();
        timer += getDelay();

        if (timer > 10) {
            refreshFlag = true;
            timer = 0;
        }

        if (refreshFlag) {
            refreshFlag = false;
            updateWindow();
            updateGame();
            if (checkGameStatus()) {
                break;
            }
        }
    }

    _getch();

    return 0;
}

void loadSunshineBallPics(int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_SUNSHINE_PATH, "Sun_", i);
        if (fileExist(fname)) {
            loadimage(&imgSunshineBallPics[i], fname);
        } else {
            break;
        }
    }
}

void loadSunflowerPics(int index, int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_SUNFLOWER_PATH, "SunFLower_", i);
        if (fileExist(fname)) {
            imgGlobalPlantsPics[index][i] = new IMAGE;
            loadimage(imgGlobalPlantsPics[index][i], fname);
        } else {
            break;
        }
    }
}

void loadPeashooterPics(int index, int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_PEASHOOTER_PATH, "Peashooter_", i);
        if (fileExist(fname)) {
            imgGlobalPlantsPics[index][i] = new IMAGE;
            loadimage(imgGlobalPlantsPics[index][i], fname);
        } else {
            break;
        }
    }
}

void loadPotatoMinePics(int index, int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_POTATOMINE_PATH, "PotatoMine_", i);
        if (fileExist(fname)) {
            imgGlobalPlantsPics[index][i] = new IMAGE;
            loadimage(imgGlobalPlantsPics[index][i], fname);
        } else {
            break;
        }
    }
}

void loadJalapenoPics(int index, int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_JALAPENO_PATH, "Jalapeno_", i);
        if (fileExist(fname)) {
            imgGlobalPlantsPics[index][i] = new IMAGE;
            loadimage(imgGlobalPlantsPics[index][i], fname);
        } else {
            break;
        }
    }
}

void loadChomperPics(int index, int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_CHOMPER_PATH, "Chomper_", i);
        if (fileExist(fname)) {
            imgGlobalPlantsPics[index][i] = new IMAGE;
            loadimage(imgGlobalPlantsPics[index][i], fname);
        } else {
            break;
        }
    }
}

void loadRepeatPeaShootPics(int index, int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_REPEATERPEA_PATH, "RepeaterPea_", i);
        if (fileExist(fname)) {
            imgGlobalPlantsPics[index][i] = new IMAGE;
            loadimage(imgGlobalPlantsPics[index][i], fname);
        } else {
            break;
        }
    }
}

void loadSnowPeaPics(int index, int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_SNOWPEA_PATH, "SnowPea_", i);
        if (fileExist(fname)) {
            imgGlobalPlantsPics[index][i] = new IMAGE;
            loadimage(imgGlobalPlantsPics[index][i], fname);
        } else {
            break;
        }
    }
}

void loadNormalZombieWalkPics(int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_NORMAL_ZOMBIE_WALK_PATH, "Zombie_", i);
        if (fileExist(fname)) {
            loadimage(&imgZombiesPics[i], fname);
        } else {
            break;
        }
    }
}

void loadNormalZombieStandPics(int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_NORMAL_ZOMBIE_STAND_PATH, "Zombie_", i);
        if (fileExist(fname)) {
            loadimage(&imgZombiesStandPics[i], fname);
        } else {
            break;
        }
    }
}

void loadNormalZombieAttackPics(int size) {
    char fname[128];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_NORMAL_ZOMBIE_ATTACK_PATH, "ZombieAttack_", i);
        if (fileExist(fname)) {
            loadimage(&imgZombiesAttackPics1[i], fname);
        } else {
            break;
        }
    }
}

void loadNormalZombieDiePics(int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_NORMAL_ZOMBIE_DIE_PATH, "ZombieDie_", i);
        if (fileExist(fname)) {
            loadimage(&imgZombiesDeadPics2[i], fname);
        } else {
            break;
        }
    }
}

void loadZombieBoomDiePics(int size) {
}

void playSounds(const char* path) {
     char play[64] = "play ";
     char* result = strcat(play, path);
    int ret = mciSendString(result, 0, 0, 0);
    cout << "event: [play] - " << result << " ret -> " << ret << endl;
 }
