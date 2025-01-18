#include "Queen.h"

Queen::Queen(char colour) : Piece{colour == 'B' ? 'q' : 'Q', colour} {}

bool Queen::isValidMove(int row, int col) const{
    if((abs(row) == abs(col)) || (row == 0) || (col == 0)){
        return true;
    }
    return false;
}
