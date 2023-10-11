//
// Created by 幻弦让叶 on 2023/10/11.
//

#ifndef PVZ_POTATOMINE_H
#define PVZ_POTATOMINE_H

#include "../Plant.h"

class PotatoMine: public Plant {
public:PotatoMine(int index) {
    this->index = index;
        this->cd = 20;
        this->hp = 100;
        this->sunshine = 25;
        this->index = index;
    }

    void loadCardPic() override {
    }

    void loadActionPics() override {
    }
};

#endif //PVZ_POTATOMINE_H
