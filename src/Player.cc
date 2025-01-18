#include "Player.h"

Player::Player(std::string id) : id{id} {}

std::string Player::getId(){
    return id;
}

Player::~Player() {}
