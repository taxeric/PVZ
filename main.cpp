#include <iostream>
#include <graphics.h>
#include <conio.h>
#include <cstdio>
#include "m_local_resources.h"
#include "models/Land.h"

using namespace std;

#define WIN_WIDTH 1030
#define WIN_HEIGHT 600

//卡槽起始坐标
#define CARD_SLOT_START_X 250
#define CARD_SLOT_START_Y 0

//卡片宽高
#define BASE_CARD_WIDTH 52
#define BASE_CARD_HEIGHT 72
//卡槽之间间距
#define SPACE_BETWEEN_CARD 2
//卡片卡槽起始坐标
#define CARD_START_X 325
#define CARD_START_Y 7

//土地行列数
#define LAND_MAP_ROW 5
#define LAND_MAP_COLUMN 9
//土地左上角起始坐标
#define LAND_MAP_START_X 250
#define LAND_MAP_START_Y 80
//土地右下角结束坐标
#define LAND_MAP_END_X 988
#define LAND_MAP_END_Y 580
//每块土地宽高
#define LAND_MAP_SINGLE_WIDTH 80
#define LAND_MAP_SINGLE_HEIGHT 100

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

IMAGE imgBg;
IMAGE imgBar;
//植物卡槽图片
IMAGE imgCards[PLANTS_COUNT];
//最大图片数量
const int max_plant_pics = 21;
//植物图片
IMAGE* imgPlants[PLANTS_COUNT][max_plant_pics];

void gameInit() {
    loadimage(&imgBg, BASE_RES_BG_PATH);
    loadimage(&imgBar, BASE_RES_BAR_BG_PATH);

    //加载植物卡槽图片
    loadimage(&imgCards[0], RES_CARD_PIC_SUNFLOWER, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
    loadimage(&imgCards[1], RES_CARD_PIC_PEASHOOTER, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
    loadimage(&imgCards[2], RES_CARD_PIC_POTATOMINE, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
    loadimage(&imgCards[3], RES_CARD_PIC_JALAPENO, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
    loadimage(&imgCards[4], RES_CARD_PIC_CHOMPER, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);
    loadimage(&imgCards[5], RES_CARD_PIC_REPEATERPEA, BASE_CARD_WIDTH, BASE_CARD_HEIGHT);

    //加载植物图片
    memset(imgPlants, 0, sizeof(imgPlants));
    memset(landMap, 0, sizeof(landMap));

    loadSunflowerPics(17);
    loadPeashooterPics(12);
    loadPotatoMinePics(12);
    loadJalapenoPics(7);
    loadChomperPics(12);
    loadRepeatPeaShootPics(14);

    initgraph(WIN_WIDTH, WIN_HEIGHT, 1);

    curMovePlantPos = 0;

    gross_card_slot_space_x = (PLANTS_COUNT - 1) * SPACE_BETWEEN_CARD;
}

void updateWindow() {
    //缓冲
    BeginBatchDraw();

    putimage(0, 0, &imgBg);
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
        putimage(x, CARD_START_Y, &imgCards[i]);
    }

    if (curMovePlantPos > 0) {
        IMAGE* img = imgPlants[curMovePlantPos - 1][0];
        putimage(curMovePlantX - img->getwidth() / 2, curMovePlantY - img->getheight() / 2, img);
    }

    //绘制土地植物
    for (int i = 0; i < LAND_MAP_ROW; i ++) {
        for (int j = 0; j < LAND_MAP_COLUMN; j ++) {
            if (landMap[i][j].type > 0) {
                //获取当前选择的植物下标
                int curPlantIndex = landMap[i][j].type - 1;
                IMAGE* img = imgPlants[curPlantIndex][landMap[i][j].frameIndex];
                int x = LAND_MAP_START_X + j * LAND_MAP_SINGLE_WIDTH + (LAND_MAP_SINGLE_WIDTH - img->getwidth()) / 2;
                int y = LAND_MAP_START_Y + i * LAND_MAP_SINGLE_HEIGHT + (LAND_MAP_SINGLE_HEIGHT - img->getheight()) / 2;
                putimage(x, y, imgPlants[curPlantIndex][landMap[i][j].frameIndex]);
            }
        }
    }

    //结束缓冲
    EndBatchDraw();
}

void userClickEvent() {
    ExMessage message{};
    static int status = 0;
    if (peekmessage(&message)) {
        if (message.message == WM_LBUTTONDOWN) {
            //鼠标按下事件
            //x范围
            bool x_value = message.x > CARD_START_X && message.x < CARD_START_X + BASE_CARD_WIDTH * PLANTS_COUNT + gross_card_slot_space_x;
            //y范围
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

void updateGame() {
    for (int i = 0; i < LAND_MAP_ROW; i ++) {
        for (int j = 0; j < LAND_MAP_COLUMN; j ++) {
            if (landMap[i][j].type > 0) {
                landMap[i][j].frameIndex ++;
                int plantIndex = landMap[i][j].type - 1;
                int frameIndex = landMap[i][j].frameIndex;
                if (imgPlants[plantIndex][frameIndex] == nullptr) {
                    landMap[i][j].frameIndex = 0;
                }
            }
        }
    }
}

int main() {
    std::cout << "Hello, PVZ!" << std::endl;

    gameInit();

    while (true) {
        userClickEvent();
        updateWindow();
        updateGame();
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

void loadSunflowerPics(int size) {
    char fname[64];
    for (int i = 0; i < size; i ++) {
        sprintf_s(fname, sizeof(fname), "%s%s%d.png", RES_PIC_SUNFLOWER_PATH, "SunFLower_", i);
        if (fileExist(fname)) {
            imgPlants[0][i] = new IMAGE;
            loadimage(imgPlants[0][i], fname);
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
            imgPlants[1][i] = new IMAGE;
            loadimage(imgPlants[1][i], fname);
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
            imgPlants[2][i] = new IMAGE;
            loadimage(imgPlants[2][i], fname);
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
            imgPlants[3][i] = new IMAGE;
            loadimage(imgPlants[3][i], fname);
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
            imgPlants[4][i] = new IMAGE;
            loadimage(imgPlants[4][i], fname);
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
            imgPlants[5][i] = new IMAGE;
            loadimage(imgPlants[5][i], fname);
        } else {
            break;
        }
    }
}
