//
// Created by 幻弦让叶 on 2023/10/9.
//

#ifndef PVZ_SUNFLOWER_H
#define PVZ_SUNFLOWER_H

#include "../Plant.h"

class Sunflower: public Plant {

public:Sunflower(std::string cardPath, std::string actionPath, int actionPicCount, int index) {
        this->setName("向日葵");
        this->setCardPath(cardPath);
        this->setActionPath(actionPath);
        this->index = index;
        this->cd = 10;
        this->hp = 100;
        this->sunshine = 50;
        this->actionPicCount = actionPicCount;
    }

    void loadCardPic() override {
    }

    void loadActionPics() override {
    }
};

#endif //PVZ_SUNFLOWER_H
