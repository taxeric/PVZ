//
// Created by 幻弦让叶 on 2023/10/9.
//

#ifndef PVZ_SUNFLOWER_H
#define PVZ_SUNFLOWER_H

#include "../Plant.h"

class RepeaterPea: public Plant {

    RepeaterPea(std::string cardPath, std::string actionPath, int actionPicCount) {
        this->setName("双发射手");
        this->setCardPath(cardPath);
        this->setActionPath(actionPath);
        this->cd = 30;
        this->hp = 100;
        this->sunshine = 200;
        this->actionPicCount = actionPicCount;
    }

    void loadCardPic() override {
    }

    void loadActionPics() override {
    }
};

#endif //PVZ_SUNFLOWER_H
