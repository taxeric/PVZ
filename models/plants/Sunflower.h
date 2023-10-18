//
// Created by 幻弦让叶 on 2023/10/9.
//

#ifndef PVZ_SUNFLOWER_H
#define PVZ_SUNFLOWER_H

#include "../Plant.h"

class Sunflower: public Plant {

public:
    Sunflower(std::string cardPath, std::string actionPath, int actionPicCount, int index) {
        this->setName("向日葵");
        this->setCardPath(cardPath);
        this->setActionPath(actionPath);
        this->index = index;
        this->cd = 10;
        this->hp = 100;
        this->sunshine = 50;
        this->actionPicCount = actionPicCount;
    }

    explicit Sunflower(Plant *plant) {
        auto* temp = dynamic_cast<Sunflower*>(plant);
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

    ~Sunflower() override= default;
};

#endif //PVZ_SUNFLOWER_H
