//
// Created by 幻弦让叶 on 2023/10/11.
//

#ifndef PVZ_POTATOMINE_H
#define PVZ_POTATOMINE_H

#include "../Plant.h"

class PotatoMine: public Plant {
public:

    /**
     * 装填计次
     */
    int loadTimer = 0;

    /**
     * 是否爆炸
     */
    bool explode = false;

    /**
     * 爆炸计次
     */
    int explodeTimer = 0;

    /**
     * 状态 0-idle 1-正在破土 2-破土完成, 装填完成
     */
    int potatoStatus = 0;

    PotatoMine(int index) {
        this->setName("土豆地雷");
        this->setCoolDown(250);
        this->cd = 0;
        this->hp = 100;
        this->sunshine = 25;
        this->index = index;
    }

    explicit PotatoMine(Plant *plant) {
        auto* temp = dynamic_cast<PotatoMine*>(plant);
        index = temp->index;
        cd = 0;
        hp = 100;
        row = temp->row;
        column = temp->column;
        sunshine = temp->sunshine;
        actionPicCount = temp->actionPicCount;
        setName(temp->getName());
        setCoolDown(temp->getCoolDown());
        loadTimer = 0;
        potatoStatus = 0;
        std::cout << "event: [new] " << temp->getName() << std::endl;
    }

    void loadCardPic() override {
    }

    void loadActionPics() override {
    }

    ~PotatoMine() override= default;
};

#endif //PVZ_POTATOMINE_H
