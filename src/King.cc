#include "King.h"

King::King(char colour) : Piece{colour == 'B' ? 'k' : 'K', colour}, isMoved{false} {}

bool King::isValidMove(int row, int col) const {
    if((abs(row) == 1 || row == 0) && (abs(col) == 1 || col == 0) && !(row == 0 && col == 0)){
        return true;
    }
    else if(row == 0 && (col == -2 || col == 2)){
        // Case for potential castle
        return true;
    }

    return false;
}

bool King::hasMoved() const{
    return isMoved;
}

void King::moved(){
    isMoved = true;
}
