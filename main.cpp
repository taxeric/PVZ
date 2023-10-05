#include <iostream>
#include <graphics.h>
#include <conio.h>
#include <cstdio>
#include <ctime>
#include "m_local_resources.h"
#include "models/Land.h"
#include "models/SunshineBall.h"

using namespace std;

#define WIN_WIDTH 900
#define WIN_HEIGHT 600
//游戏界面偏移
#define WIN_OFFSET 130

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
//阳光图片数
const int max_sunshine_pics = 21;
IMAGE imgSunshineBallPics[max_sunshine_pics];
//阳光总数
int gross_sunshine;
//阳光pic宽高
int sunshinePicWidth, sunshinePicHeight;

IMAGE imgBg;
IMAGE imgBar;
//植物卡槽图片
IMAGE imgCardsPics[PLANTS_COUNT];
//最大图片数量
const int max_plant_pics = 21;
//植物图片
IMAGE* imgPlantsPics[PLANTS_COUNT][max_plant_pics];

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

void gameInit() {
    loadimage(&imgBg, BASE_RES_BG_PATH);
    loadimage(&imgBar, BASE_RES_BAR_BG_PATH);

    memset(sunshineBalls, 0, sizeof(sunshineBalls));
    //加载阳光图片
    loadSunshineBallPics(max_sunshine_pics);
    //设置图片宽高
    sunshinePicWidth = imgSunshineBallPics[0].getwidth();
    sunshinePicHeight = imgSunshineBallPics[0].getheight();
    if (sunshinePicWidth <= 0 || sunshinePicHeight <= 0) {
        sunshinePicWidth = 80;
        sunshinePicHeight = 80;
    }
    //阳光随机数种子
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

void updateWindow() {
    //缓冲
    BeginBatchDraw();

    putimage(-130, 0, &imgBg);
    putimage(CARD_SLOT_START_X, CARD_SLOT_START_Y, &imgBar);
    setbkcolor(TRANSPARENT);

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

    //绘制土地植物
    for (int i = 0; i < LAND_MAP_ROW; i ++) {
        for (int j = 0; j < LAND_MAP_COLUMN; j ++) {
            if (landMap[i][j].type > 0) {
                //获取当前选择的植物下标
                int curPlantIndex = landMap[i][j].type - 1;
                IMAGE* img = imgPlantsPics[curPlantIndex][landMap[i][j].frameIndex];
                int x = LAND_MAP_START_X + j * LAND_MAP_SINGLE_WIDTH + (LAND_MAP_SINGLE_WIDTH - img->getwidth()) / 2;
                int y = LAND_MAP_START_Y + i * LAND_MAP_SINGLE_HEIGHT + (LAND_MAP_SINGLE_HEIGHT - img->getheight()) / 2;
                putimage(x, y, imgPlantsPics[curPlantIndex][landMap[i][j].frameIndex]);
            }
        }
    }

    if (curMovePlantPos > 0) {
        IMAGE* img = imgPlantsPics[curMovePlantPos - 1][0];
        putimage(curMovePlantX - img->getwidth() / 2, curMovePlantY - img->getheight() / 2, img);
    }

    int ballMax = sizeof(sunshineBalls) / sizeof(sunshineBalls[0]);
    for (int i = 0; i < ballMax; i ++) {
        if (sunshineBalls[i].isUsed) {
            IMAGE* sunshineImg = &imgSunshineBallPics[sunshineBalls[i].frameIndex];
            putimage(sunshineBalls[i].x, sunshineBalls[i].y, sunshineImg);
        }
    }

    char scoreText[8];
    sprintf_s(scoreText, sizeof(scoreText), "%d", gross_sunshine);
    outtextxy(SUNSHINE_TEXT_START_X, SUNSHINE_TEXT_START_Y, scoreText);

    //结束缓冲
    EndBatchDraw();
}

void collectSunshine(ExMessage* message) {
    int count = sizeof(sunshineBalls) / sizeof(sunshineBalls[0]);
    for (int i = 0; i < count; i ++) {
        if (sunshineBalls[i].isUsed) {
            int x = sunshineBalls[i].x;
            int y = sunshineBalls[i].y;
            bool x_value = message->x > x && message->x < x + sunshinePicWidth;
            bool y_value = message->y > y && message->y < y + sunshinePicHeight;
            if (x_value && y_value) {
                sunshineBalls[i].isUsed = false;
                gross_sunshine += SUNSHINE_AMOUNT;
//                cout << "sunshine amount = " << gross_sunshine << endl;
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
                        cout << "event: [up] (" << row << "," << column << ") plant index = " << landMap[row][column].type << endl;
                    }
                }
                status = 0;
                curMovePlantPos = 0;
            }
        }
    }
}

void createSunshine() {
    static int count = 0;
    static int fre = 400;
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
        sunshineBalls[i].isUsed = true;
        sunshineBalls[i].frameIndex = 0;
        sunshineBalls[i].x = LAND_MAP_START_X + rand() % (LAND_MAP_END_X - LAND_MAP_START_X);
//    sunshineBalls[i].y = LAND_MAP_START_Y + rand() % (LAND_MAP_END_Y - LAND_MAP_START_Y);
        sunshineBalls[i].y = LAND_MAP_START_Y;
        sunshineBalls[i].destY = LAND_MAP_START_Y + (rand() % 4) * 90;
        sunshineBalls[i].timer = 0;
//        cout << "produce gross_sunshine - " << i << " (" << sunshineBalls[i].x << "," << sunshineBalls[i].y << "," << sunshineBalls[i].destY << ")" << endl;
    }
}

void updateSunshine() {
    int ballMax = sizeof(sunshineBalls) / sizeof(sunshineBalls[0]);
    for (int i = 0; i < ballMax; i ++) {
        if (sunshineBalls[i].isUsed) {
            sunshineBalls[i].frameIndex = (sunshineBalls[i].frameIndex + 1) % max_sunshine_pics;
            if (sunshineBalls[i].timer == 0) {
                sunshineBalls[i].y += 2;
            }
            if (sunshineBalls[i].y >= sunshineBalls[i].destY) {
                sunshineBalls[i].timer ++;
                if (sunshineBalls[i].timer > 100) {
                    sunshineBalls[i].isUsed = false;
                }
            }
        }
    }
}

void updateGame() {
    for (int i = 0; i < LAND_MAP_ROW; i ++) {
        for (int j = 0; j < LAND_MAP_COLUMN; j ++) {
            if (landMap[i][j].type > 0) {
                landMap[i][j].frameIndex ++;
                int plantIndex = landMap[i][j].type - 1;
                int frameIndex = landMap[i][j].frameIndex;
                if (imgPlantsPics[plantIndex][frameIndex] == nullptr) {
                    landMap[i][j].frameIndex = 0;
                }
            }
        }
    }

    createSunshine();
    updateSunshine();
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
        putimage(480, 80, move_flag ? &imgAdventure1 : &imgAdventure0);

        ExMessage message{};
        if (peekmessage(&message)) {
            if (message.message == WM_LBUTTONUP && action_flag) {
                return;
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

int main() {
    std::cout << "Hello, PVZ!" << std::endl;

    gameInit();
    startMenuUI();

    int timer = 0;
    bool refreshFlag = true;
    while (true) {

        userClickEvent();
        timer += getDelay();

        if (timer > 30) {
            refreshFlag = true;
            timer = 0;
        }

        if (refreshFlag) {
            refreshFlag = false;
            updateWindow();
            updateGame();
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
