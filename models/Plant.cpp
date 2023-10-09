//
// Created by 幻弦让叶 on 2023/10/9.
//

#include "Plant.h"

const std::string &Plant::getName() const {
    return name;
}

void Plant::setName(const std::string &name) {
    Plant::name = name;
}

const std::string &Plant::getCardPath() const {
    return cardPath;
}

void Plant::setCardPath(const std::string &cardPath) {
    Plant::cardPath = cardPath;
}

const std::string &Plant::getActionPath() const {
    return actionPath;
}

void Plant::setActionPath(const std::string &path) {
    Plant::actionPath = path;
}
