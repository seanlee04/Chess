#include "Piece.h"

Piece::Piece(char p, char c) : name{p}, colour{c} {}

char Piece::getName() const{
    return name;
}

char Piece::getColour() const{
    return colour;
}

bool Piece::operator==(const Piece &other) const {
    if(name == other.name && colour == other.colour){
        return true;
    }
    return false;
}
