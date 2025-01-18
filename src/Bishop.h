#ifndef BISHOP_H
#define BISHOP_H

#include "Piece.h"

class Bishop : public Piece{
    public:
        bool isValidMove(int row, int col) const override;
        explicit Bishop(char colour);
};

#endif
