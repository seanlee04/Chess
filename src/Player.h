#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player{
    std::string id;
    public:
        explicit Player(std::string id);
        virtual std::string getId();
        virtual ~Player() = 0;
};

#endif
