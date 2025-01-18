#include "Rook.h"

Rook::Rook(char colour) : Piece{colour == 'B' ? 'r' : 'R', colour} {}

bool Rook::isValidMove(int row, int col) const {
    if((row == 0) || (col == 0)){
        return true;
    }
    return false;
}

bool Rook::hasMoved() const{
    return isMoved;
}

void Rook::moved(){
    isMoved = true;
}
