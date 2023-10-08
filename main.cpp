#include <iostream>
#include <graphics.h>
#include <conio.h>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <vector>
#include "m_local_resources.h"
#include "models/Land.h"
#include "models/SunshineBall.h"
#include "models/Zombie.h"
#include "models/Bullet.h"
#include "models/GameStatus.h"

using namespace std;

#define WIN_WIDTH 900
#define WIN_HEIGHT 600
//游戏界面偏移
#define WIN_OFFSET 130
//图片资源最大数量
#define BASE_RES_PICS_AMOUNT 21

//卡槽起始坐标
#define CARD_SLOT_START_X (250 - WIN_OFFSET)
#define CARD_SLOT_START_Y 0

//卡片宽高
#define BASE_CARD_WIDTH 52
#define BASE_CARD_HEIGHT 72
//卡槽之间间距
#define SPACE_BETWEEN_CARD 2
//卡片卡槽起始坐标
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
#define AMOUNT_ZOMBIE_ATTACK_PIC_1 20
//僵尸死亡图片数量
#define AMOUNT_ZOMBIE_DEAD_PIC_1 17
#define AMOUNT_ZOMBIE_DEAD_PIC_2 9

//当前关卡
int game_level;
int game_level_index;
//游戏状态
struct GameStatus gameStatus[5];

//是否首次绘制
bool isFirstDraw = true;
//卡槽之间的间距总和
int gross_card_slot_space_x = 0;
//卡槽起点终点x坐标
int card_slot_x_coordinate[PLANTS_COUNT][2];

//当前选中植物在移动过程中的位置
int curMovePlantX, curMovePlantY;
//当前移动的植物位置, 从1开始, 用于判断是否选中植物, 0->未选择植物
int curMovePlantPos;

//土地
struct Land landMap[LAND_MAP_ROW][LAND_MAP_COLUMN];
//阳光池
struct SunshineBall sunshineBalls[10];
//阳光图片
IMAGE imgSunshineBallPics[BASE_RES_PICS_AMOUNT];
//阳光总数
int gross_sunshine;
//阳光pic宽高
int sunshinePicWidth, sunshinePicHeight;

IMAGE imgBg;
IMAGE imgBar;
//植物卡槽图片
IMAGE imgCardsPics[PLANTS_COUNT];
//植物图片
IMAGE* imgPlantsPics[PLANTS_COUNT][BASE_RES_PICS_AMOUNT];

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

//子弹池
struct Bullet bullets[30];
//正常豌豆子弹
IMAGE imgBulletNormal;
//豌豆子弹碰撞后
IMAGE imgBulletNormalExplode[4];

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

void putimagePng(int x, int y, IMAGE* image) {
    DWORD* dst = GetImageBuffer();
    DWORD* draw = GetImageBuffer();
    DWORD* src = GetImageBuffer(image);
    int pw = image->getwidth();
    int ph = image->getheight();
    int gw = getwidth();
    int gh = getheight();
    int dstX = 0;
    for (int iy = 0; iy < ph; iy ++) {
        for (int ix = 0; ix < pw; ix ++) {
            int srcX = ix + iy * pw;
            int sa = ((src[srcX] & 0xFF000000) >> 24);
            int sr = ((src[srcX] & 0xFF0000) >> 16);
            int sg = ((src[srcX] & 0xFF00) >> 8);
            int sb = ((src[srcX] & 0xFF));
            if (ix >= 0 && ix <= gw && iy >= 0 && iy <= gh && dstX <= gw * gh) {
                dstX = (ix + x) + (iy + y) * gw;
                int dr = ((dst[dstX] & 0xFF0000) >> 16);
                int dg = ((dst[dstX] & 0xFF00) >> 8);
                int db = ((dst[dstX] & 0xFF));
                draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16) | ((sg * sa / 255 + dg * (255 - sa) / 255) << 8) | (sb * sa / 255 + db * (255 - sa) / 255);
            }
        }
    }
}

void gameInit() {
    loadimage(&imgBg, BASE_RES_BG_PATH);
    loadimage(&imgBar, BASE_RES_BAR_BG_PATH);

    game_level = 0;
    gameStatus[game_level].levelStatus = GameIdle;
    gameStatus[game_level].level = game_level + 1;
    gameStatus[game_level].killCount = 0;
    gameStatus[game_level].zombieMaxCount = 10;

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
    loadimage(&imgCardsPics[0], RES_CARD_PIC_SUNFLOWER, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
    loadimage(&imgCardsPics[1], RES_CARD_PIC_PEASHOOTER, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
    loadimage(&imgCardsPics[2], RES_CARD_PIC_POTATOMINE, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
    loadimage(&imgCardsPics[3], RES_CARD_PIC_JALAPENO, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
    loadimage(&imgCardsPics[4], RES_CARD_PIC_CHOMPER, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
    loadimage(&imgCardsPics[5], RES_CARD_PIC_REPEATERPEA, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);

    //土地
    memset(landMap, 0, sizeof(landMap));

    //加载植物图片
    memset(imgPlantsPics, 0, sizeof(imgPlantsPics));
    loadSunflowerPics(17);
    loadPeashooterPics(12);
    loadPotatoMinePics(12);
    loadJalapenoPics(7);
    loadChomperPics(12);
    loadRepeatPeaShootPics(14);

    //加载僵尸数据
    memset(zombies, 0, sizeof(zombies));
    loadNormalZombieWalkPics(21);
    loadNormalZombieAttackPics(AMOUNT_ZOMBIE_ATTACK_PIC_1);
    loadNormalZombieStandPics(AMOUNT_ZOMBIE_STAND_PIC_1);
    loadNormalZombieDiePics(AMOUNT_ZOMBIE_DEAD_PIC_2);

    //加载子弹数据
    memset(bullets, 0, sizeof(bullets));
    loadimage(&imgBulletNormal, RES_PIC_BULLET_PEA_NORMAL);
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

    initgraph(WIN_WIDTH, WIN_HEIGHT, 1);

    curMovePlantPos = 0;
    gross_sunshine = 50;

    gross_card_slot_space_x = (PLANTS_COUNT - 1) * SPACE_BETWEEN_CARD;

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
 * 绘制土地植物
 */
void drawPlants() {
    for (int i = 0; i < LAND_MAP_ROW; i ++) {
        for (int j = 0; j < LAND_MAP_COLUMN; j ++) {
            if (landMap[i][j].type > 0) {
                //获取当前选择的植物下标
                int curPlantIndex = landMap[i][j].type - 1;
                IMAGE* img = imgPlantsPics[curPlantIndex][landMap[i][j].frameIndex];
                int x = LAND_MAP_START_X + j * LAND_MAP_SINGLE_WIDTH + (LAND_MAP_SINGLE_WIDTH - img->getwidth()) / 2;
                int y = LAND_MAP_START_Y + i * LAND_MAP_SINGLE_HEIGHT + (LAND_MAP_SINGLE_HEIGHT - img->getheight()) / 2;
                putimagePng(x, y, imgPlantsPics[curPlantIndex][landMap[i][j].frameIndex]);
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
            putimagePng(zombies[i].x, zombies[i].y - img->getheight(), img);
        }
    }
}

/**
 * 绘制子弹
 */
void drawBullets() {
    int bulletsMax = sizeof(bullets) / sizeof(bullets[0]);
    for (int i = 0; i < bulletsMax; i ++) {
        if (bullets[i].isUsed) {
            if (bullets[i].explosion) {
                IMAGE *img = &imgBulletNormalExplode[bullets[i].frameIndex];
                putimagePng(bullets[i].x, bullets[i].y, img);
            } else {
                putimagePng(bullets[i].x, bullets[i].y, &imgBulletNormal);
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
            putimagePng(sunshineBalls[i].x, sunshineBalls[i].y, sunshineImg);
        }
    }
}

void updateWindow() {
    //缓冲
    BeginBatchDraw();

    putimage(- WIN_OFFSET, 0, &imgBg);
    putimage(CARD_SLOT_START_X, CARD_SLOT_START_Y, &imgBar);
    setbkcolor(TRANSPARENT);

    //绘制卡槽
    int space_x = 0;
    for (int i = 0; i < PLANTS_COUNT; i ++) {
        long int x = CARD_START_X + i * BASE_CARD_WIDTH;
        if (i > 0) {
            space_x += SPACE_BETWEEN_CARD;
            x += space_x;
        }
        if (card_slot_x_coordinate[i][0] <= 0 && card_slot_x_coordinate[i][1] <= 0) {
//            cout << "reset coordinate" << endl;
            card_slot_x_coordinate[i][0] = x;
            card_slot_x_coordinate[i][1] = x + BASE_CARD_WIDTH;
        }
        putimage(x, CARD_START_Y, &imgCardsPics[i]);
    }

    //拖动绘制
    if (curMovePlantPos > 0) {
        IMAGE* img = imgPlantsPics[curMovePlantPos - 1][0];
        putimagePng(curMovePlantX - img->getwidth() / 2, curMovePlantY - img->getheight() / 2, img);
    }

    char scoreText[8];
    sprintf_s(scoreText, sizeof(scoreText), "%d", gross_sunshine);
    outtextxy(SUNSHINE_TEXT_START_X, SUNSHINE_TEXT_START_Y, scoreText);

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
                gross_sunshine += SUNSHINE_AMOUNT;
//                cout << "sunshine amount = " << gross_sunshine << endl;
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
    if (peekmessage(&message)) {
        if (message.message == WM_LBUTTONDOWN) {
            //鼠标按下事件
            //植物卡槽x范围
            bool x_value = message.x > CARD_START_X && message.x < CARD_START_X + BASE_CARD_WIDTH * PLANTS_COUNT + gross_card_slot_space_x;
            //植物卡槽y范围
            bool y_value = message.y > CARD_START_Y && message.y < BASE_CARD_HEIGHT;
            if (x_value && y_value) {
                for (int x_index = 0; x_index < PLANTS_COUNT; x_index ++) {
                    if (message.x > card_slot_x_coordinate[x_index][0] && message.x < card_slot_x_coordinate[x_index][1]) {
//                        cout << "valid click -> " << x_index << endl;
                        status = 1;
                        curMovePlantX = message.x;
                        curMovePlantY = message.y;
                        curMovePlantPos = x_index + 1;
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
//                    Land curLand = ;
                    if (landMap[row][column].type <= 0) {
                        landMap[row][column].type = curMovePlantPos;
                        landMap[row][column].frameIndex = 0;
                        landMap[row][column].caught = false;
                        landMap[row][column].deadTime = 10;
                        landMap[row][column].x = LAND_MAP_START_X + column * LAND_MAP_SINGLE_WIDTH;
                        landMap[row][column].y = LAND_MAP_START_Y + row * LAND_MAP_SINGLE_HEIGHT;
                        cout << "event: [up] (" << row << "," << column << ") plant index = " << landMap[row][column].type << endl;
                    }
                }
                status = 0;
                curMovePlantPos = 0;
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
                    if (imgPlantsPics[plantIndex][frameIndex] == nullptr) {
                        landMap[i][j].frameIndex = 0;
                    }
                }
            }
        }
    }
}

void createSunshine() {
    static int count = 0;
    static int fre = 80;
    count ++;
    if (count >= fre) {
        fre = 200 + rand() % 20;
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
//    sunshineBalls[i].y = LAND_MAP_START_Y + rand() % (LAND_MAP_END_Y - LAND_MAP_START_Y);
        sunshineBall->y = LAND_MAP_START_Y;
        sunshineBall->destY = LAND_MAP_START_Y + (rand() % 4) * 90;
        sunshineBall->timer = 0;
        sunshineBall->xOffset = 0;
        sunshineBall->yOffset = 0;
        sunshineBall->status = SUNSHINE_DOWN;
//        cout << "produce gross_sunshine - " << i << " (" << sunshineBalls[i].x << "," << sunshineBalls[i].y << "," << sunshineBalls[i].destY << ")" << endl;
    }

    int sunshineBallMax = sizeof(sunshineBalls) / sizeof(sunshineBalls[0]);
    for (int row = 0; row < LAND_MAP_ROW; row ++) {
        for (int column = 0; column < LAND_MAP_COLUMN; column ++) {
            if (landMap[row][column].type - 1 == SUNFLOWER) {
                landMap[row][column].timer ++;
                if (landMap[row][column].timer > 200) {
                    landMap[row][column].timer = 0;
                    int k;
                    for (k = 0; k < sunshineBallMax && sunshineBalls[k].isUsed; k ++);
                    if (k >= sunshineBallMax) {
                        return;
                    }
                    IMAGE* sunflowerImg = imgPlantsPics[SUNFLOWER][0];
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
        if (sunshineBalls[i].isUsed) {
            struct SunshineBall* sunshineBall = &sunshineBalls[i];
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
        } else if (sunshineBalls[i].xOffset > 0) {
            //设置偏移
            float destX = CARD_SLOT_START_X;
            float destY = CARD_SLOT_START_Y;
            float angle = atan(((float) sunshineBalls[i].y - destY) / ((float) sunshineBalls[i].x - destX));
            sunshineBalls[i].xOffset = SUNSHINE_FLY_PIXEL * cos(angle);
            sunshineBalls[i].yOffset = SUNSHINE_FLY_PIXEL * sin(angle);

            sunshineBalls[i].x -= sunshineBalls[i].xOffset;
            sunshineBalls[i].y -= sunshineBalls[i].yOffset;
            if (sunshineBalls[i].x <= CARD_SLOT_START_X || sunshineBalls[i].y <= CARD_SLOT_START_Y) {
                sunshineBalls[i].xOffset = 0;
                sunshineBalls[i].yOffset = 0;
//                gross_sunshine += SUNSHINE_AMOUNT;
            }
        }
    }
}

void createZombies() {
    if (gameStatus[game_level].zombieCount >= gameStatus[game_level].zombieMaxCount) {
        return;
    }

    static int zombieFre = 400;//僵尸生成间隔
    static int count = 0;
    count ++;
    if (count > zombieFre) {
        count = 0;
        zombieFre = rand() % 200 + 300;
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
            gameStatus[game_level].zombieCount ++;
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
                        zombies[i].x -= zombies[i].speed;
                    }
                }
                if (zombies[i].x < LAND_MAP_START_X - 80) {//僵尸进入房子了🧠
                    //game over ~~~
                    cout << "game over ~~~" << endl;
                    gameStatus[game_level].levelStatus = GameFailed;
//                    exit(0);
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
                } else if (zombies[i].eating) {
                    zombies[i].frameIndex = (zombies[i].frameIndex + 1) % AMOUNT_ZOMBIE_ATTACK_PIC_1;
                } else {
                    zombies[i].frameIndex = (zombies[i].frameIndex + 1) % BASE_RES_PICS_AMOUNT;
                }
            }
        }
    }
}

void plantsShoot() {
    int lines[LAND_MAP_ROW] = {0};
    int zombieCount = sizeof(zombies) / sizeof(zombies[0]);
    int dangerX = LAND_MAP_END_X - 80/* - imgZombiesPics[0].getwidth()*/;//手动减去僵尸png前方的占位透明像素
    int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
    for (int i = 0; i < zombieCount; i ++) {
        if (zombies[i].isUsed && zombies[i].x < dangerX) {
            lines[zombies[i].row] = 1;
        }
    }

    for (int i = 0; i < LAND_MAP_ROW; i ++) {
        for (int j = 0; j < LAND_MAP_COLUMN; j ++) {
            if (landMap[i][j].type == 0) {
                continue;
            }
            if (landMap[i][j].type - 1 == PEASHOOT && lines[i]) {
                static int count = 0;
                count ++;
                if (count > 120) {//越大子弹间隔越大
                    count = 0;
                    int k;
                    //找到可用的子弹
                    for (k = 0; k < bulletMax && bullets[k].isUsed; k ++);
                    if (k < bulletMax) {
                        bullets[k].isUsed = true;
                        bullets[k].row = i;
                        bullets[k].speed = 4;

                        bullets[k].explosion = false;
                        bullets[k].frameIndex = 0;

                        int plantX = LAND_MAP_START_X + j * LAND_MAP_SINGLE_WIDTH;
                        int plantY = LAND_MAP_START_Y + i * LAND_MAP_SINGLE_HEIGHT;
                        bullets[k].x = plantX + imgPlantsPics[landMap[i][j].type - 1][0]->getwidth() - 10;
                        bullets[k].y = plantY + 5;
                    }
                }
            }
        }
    }
}

void updateBullets() {
    int countMax = sizeof(bullets) / sizeof(bullets[0]);
    for (int i = 0; i < countMax; i ++) {
        if (bullets[i].isUsed) {
            bullets[i].x = bullets[i].x + bullets[i].speed;
            if (bullets[i].x > LAND_MAP_END_X) {
                bullets[i].isUsed = false;
            }
            if (bullets[i].explosion) {
                bullets[i].frameIndex ++;
                if (bullets[i].frameIndex >= 4) {
                    bullets[i].isUsed = false;
                }
            }
        }
    }
}

void checkBullet2Zombie() {
    int bulletCount = sizeof(bullets) / sizeof(bullets[0]);
    int zombieCount = sizeof(zombies) / sizeof(zombies[0]);
    for (int i = 0; i < bulletCount; i ++) {
        if (!bullets[i].isUsed || bullets[i].explosion) {
            continue;
        }
        for (int k = 0; k < zombieCount; k ++) {
            if (!zombies[k].isUsed) {
                continue;
            }
            int zombieX1 = zombies[k].x + 80;//僵尸图片实际需要碰撞的位置起点x, 因为图片尺寸需要手动加上偏移
            int zombieX2 = zombies[k].x + 110;//僵尸图片实际需要碰撞的位置终点x, 因为图片尺寸需要手动加上偏移
            int bulletX = bullets[i].x;
            if (!zombies[k].dead && bulletX >= zombieX1 && bulletX <= zombieX2 && bullets[i].row == zombies[k].row) {
                zombies[k].hp -= 20;//默认伤害
                bullets[i].explosion = true;
                bullets[i].speed = 0;
                if (zombies[k].hp <= 40 && zombies[k].hp > 0) {
                    zombies[k].lostHead = true;
                } else if (zombies[k].hp <= 0) {
                    zombies[k].dead = true;
                    zombies[k].speed = 0;
                    zombies[k].frameIndex = 0;
//                    zombies[k].isUsed = false;
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
                            if (count > 20) {//越大切换图片越慢
                                count = 0;
                                zombies[i].frameIndex ++;
                            }
                            landMap[row][column].deadTime ++;
                            if (landMap[row][column].deadTime > 100) {
                                landMap[row][column].deadTime = 0;
                                landMap[row][column].caught = false;
                                landMap[row][column].type = 0;
                                zombies[i].eating = false;
                                zombies[i].speed = 1;
                                zombies[i].frameIndex = 0;
                            } else {
                                if (zombies[i].frameIndex >= AMOUNT_ZOMBIE_ATTACK_PIC_1) {
                                    zombies[i].frameIndex = 0;
                                }
                            }
                        } else {
                            landMap[row][column].deadTime = 0;
                            landMap[row][column].caught = true;
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

void collisionCheck() {
    //子弹碰撞僵尸
    checkBullet2Zombie();
    //僵尸攻击植物
    checkZombie2Plant();
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
        putimagePng(480, 80, move_flag ? &imgAdventure1 : &imgAdventure0);

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
            putimagePng(
                    zombiesStandCoordinate[k][0] - xMin + x,
                    zombiesStandCoordinate[k][1],
                    &imgZombiesStandPics[0]
                    );
        }
        EndBatchDraw();
        Sleep(5);
    }

/*    while (true) {
        BeginBatchDraw();
        ExMessage message{};
        if (peekmessage(&message)) {
        }
        EndBatchDraw();
    }*/
    for (int i = 0; i < 100; i ++) {
        BeginBatchDraw();
        putimage(xMin, 0, &imgBg);
        for (int k = 0; k < 9; k ++) {
            int frameIndex = rand() % AMOUNT_ZOMBIE_STAND_PIC_1;
            putimagePng(
                    zombiesStandCoordinate[k][0],
                    zombiesStandCoordinate[k][1],
                    &imgZombiesStandPics[frameIndex]
            );
        }
        EndBatchDraw();
        Sleep(30);
    }

    int count = 0;
    int frameIndex = rand() % AMOUNT_ZOMBIE_STAND_PIC_1;
    for (int x = xMin; x <= -WIN_OFFSET; x += 1) {
        BeginBatchDraw();
        putimage(x, 0, &imgBg);
        count ++;
        for (int k = 0; k < 9; k ++) {
            putimagePng(
                    zombiesStandCoordinate[k][0] - xMin + x,
                    zombiesStandCoordinate[k][1],
                    &imgZombiesStandPics[frameIndex]
                    );
            if (count >= 10) {
                count = 0;
                frameIndex = rand() % AMOUNT_ZOMBIE_STAND_PIC_1;
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
        for (int i = 0; i < PLANTS_COUNT; i ++) {
            long int x = CARD_START_X + i * BASE_CARD_WIDTH;
            if (i > 0) {
                space_x += SPACE_BETWEEN_CARD;
                x += space_x;
            }
            if (card_slot_x_coordinate[i][0] <= 0 && card_slot_x_coordinate[i][1] <= 0) {
                card_slot_x_coordinate[i][0] = x;
                card_slot_x_coordinate[i][1] = x + BASE_CARD_WIDTH;
            }
            putimage(x, y, &imgCardsPics[i]);
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

bool fileExist(const char * filename) {
    bool exist = false;
    FILE* fp;
    fopen_s(&fp, filename, "r");
    if (fp != nullptr) {
        exist = true;
        fclose(fp);
    }
    return exist;
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

void loadSunflowerPics(int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_SUNFLOWER_PATH, "SunFLower_", i);
        if (fileExist(fname)) {
            imgPlantsPics[0][i] = new IMAGE;
            loadimage(imgPlantsPics[0][i], fname);
        } else {
            break;
        }
    }
}

void loadPeashooterPics(int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_PEASHOOTER_PATH, "Peashooter_", i);
        if (fileExist(fname)) {
            imgPlantsPics[1][i] = new IMAGE;
            loadimage(imgPlantsPics[1][i], fname);
        } else {
            break;
        }
    }
}

void loadPotatoMinePics(int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_POTATOMINE_PATH, "PotatoMine_", i);
        if (fileExist(fname)) {
            imgPlantsPics[2][i] = new IMAGE;
            loadimage(imgPlantsPics[2][i], fname);
        } else {
            break;
        }
    }
}

void loadJalapenoPics(int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_JALAPENO_PATH, "Jalapeno_", i);
        if (fileExist(fname)) {
            imgPlantsPics[3][i] = new IMAGE;
            loadimage(imgPlantsPics[3][i], fname);
        } else {
            break;
        }
    }
}

void loadChomperPics(int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_CHOMPER_PATH, "Chomper_", i);
        if (fileExist(fname)) {
            imgPlantsPics[4][i] = new IMAGE;
            loadimage(imgPlantsPics[4][i], fname);
        } else {
            break;
        }
    }
}

void loadRepeatPeaShootPics(int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_REPEATERPEA_PATH, "RepeaterPea_", i);
        if (fileExist(fname)) {
            imgPlantsPics[5][i] = new IMAGE;
            loadimage(imgPlantsPics[5][i], fname);
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
