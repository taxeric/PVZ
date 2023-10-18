//
// Created by 幻弦让叶 on 2023/10/9.
//

#ifndef PVZ_PEASHOOTER_H
#define PVZ_PEASHOOTER_H

#include "../Plant.h"

class Peashooter: public Plant {

public:
    Peashooter(std::string cardPath, std::string actionPath, int actionPicCount, int index) {
        this->setName("豌豆射手");
        this->setCardPath(cardPath);
        this->setActionPath(actionPath);
        this->index = index;
        this->cd = 10;
        this->hp = 100;
        this->sunshine = 100;
        this->actionPicCount = actionPicCount;
    }

    explicit Peashooter(Plant *plant) {
        auto* temp = dynamic_cast<Peashooter*>(plant);
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

    ~Peashooter() override= default;
};

#endif //PVZ_PEASHOOTER_H
