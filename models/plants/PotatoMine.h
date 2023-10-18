//
// Created by 幻弦让叶 on 2023/10/11.
//

#ifndef PVZ_POTATOMINE_H
#define PVZ_POTATOMINE_H

#include "../Plant.h"

class PotatoMine: public Plant {
public:
    PotatoMine(int index) {
        this->setName("土豆地雷");
        this->cd = 20;
        this->hp = 100;
        this->sunshine = 25;
        this->index = index;
    }

    explicit PotatoMine(Plant *plant) {
        auto* temp = dynamic_cast<PotatoMine*>(plant);
        index = temp->index;
        cd = temp->cd;
        hp = temp->hp;
        row = temp->row;
        column = temp->column;
        sunshine = temp->sunshine;
        actionPicCount = temp->actionPicCount;
        setName(temp->getName());
        std::cout << "event: [new] " << temp->getName() << std::endl;
    }

    void loadCardPic() override {
    }

    void loadActionPics() override {
    }

    ~PotatoMine() override= default;
};

#endif //PVZ_POTATOMINE_H
