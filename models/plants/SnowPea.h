//
// Created by 幻弦让叶 on 2023/10/12.
//

#ifndef PVZ_SNOWPEA_H
#define PVZ_SNOWPEA_H

#include "../Plant.h"

class SnowPea: public Plant{

public:
    SnowPea(int index) {
        this->setName("寒冰射手");
        this->cd = 30;
        this->hp = 100;
        this->sunshine = 175;
        this->index = index;
    }

    explicit SnowPea(Plant *plant) {
        auto* temp = dynamic_cast<SnowPea*>(plant);
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

    void loadActionPics() override{}
    void loadCardPic() override{}

    ~SnowPea() override= default;
};

#endif //PVZ_SNOWPEA_H
