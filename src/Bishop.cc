#include "Bishop.h"

Bishop::Bishop(char colour) : Piece{colour == 'B' ? 'b' : 'B', colour} {}

bool Bishop::isValidMove(int row, int col) const{
    if(abs(col) == abs(row)){
        return true;
    }
    return false;
}
