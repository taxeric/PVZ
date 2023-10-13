//
// Created by 幻弦让叶 on 2023/10/12.
//

#ifndef PVZ_SNOWPEA_H
#define PVZ_SNOWPEA_H

#include "../Plant.h"

class SnowPea: public Plant{

public:SnowPea(int index) {
        this->index = index;
        this->cd = 30;
        this->hp = 100;
        this->sunshine = 175;
        this->index = index;
}

    void loadActionPics() override{}
    void loadCardPic() override{}
};

#endif //PVZ_SNOWPEA_H
