#include "Pawn.h"

Pawn::Pawn(char colour) : Piece{colour == 'B' ? 'p' : 'P', colour}, enPassantEnabled{false} {}

bool Pawn::isValidMove(int row, int col) const {
    // pawn can only move forward 2 squares on its first move might have to fix, leaving this here
    if(col == 0 && (row == 1 || row == 2)){
        return true;
    }
    else if(abs(col) == 1 && row == 1){
        // Pawn capture
        return true;
    }
    return false;
}

bool Pawn::enPassantAllowed() const{
    return enPassantEnabled;
}

void Pawn::setEnPassant(bool value){
    enPassantEnabled = value;
}
