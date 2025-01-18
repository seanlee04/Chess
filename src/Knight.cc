#include "Knight.h"

Knight::Knight(char colour) : Piece{colour == 'B' ? 'n' : 'N', colour} {}

bool Knight::isValidMove(int row, int col) const{
    if((abs(col) == 1 && abs(row) == 2) || (abs(col) == 2 && abs(row) == 1)){
        return true;
    }
    return false;
}
