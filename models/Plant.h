//
// Created by 幻弦让叶 on 2023/10/9.
//

#ifndef PVZ_PLANT_H
#define PVZ_PLANT_H

#include <iostream>

class Plant {
private:
    std::string name;
    std::string cardPath;
    std::string actionPath;

public:
    int actionPicCount;
    int sunshine;
    int hp;
    int cd;
    int type;
    int environment;

    const std::string &getName() const;

    void setName(const std::string &name);

    const std::string &getCardPath() const;

    void setCardPath(const std::string &cardPath);

    const std::string &getActionPath() const;

    void setActionPath(const std::string &path);

    virtual void loadCardPic() = 0;
    virtual void loadActionPics() = 0;

};

#endif //PVZ_PLANT_H