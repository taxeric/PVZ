//
// Created by 幻弦让叶 on 2023/10/9.
//

#ifndef PVZ_PEASHOOTER_H
#define PVZ_PEASHOOTER_H

#include "../Plant.h"

class Peashooter: public Plant {

    Peashooter(std::string cardPath, std::string actionPath, int actionPicCount) {
        this->setName("豌豆射手");
        this->setCardPath(cardPath);
        this->setActionPath(actionPath);
        this->cd = 10;
        this->hp = 100;
        this->sunshine = 100;
        this->actionPicCount = actionPicCount;
    }

    void loadCardPic() override {
    }

    void loadActionPics() override {
    }
};

#endif //PVZ_PEASHOOTER_H
