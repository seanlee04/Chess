#ifndef KNIGHT_H
#define KNIGHT_H

#include "Piece.h"

class Knight : public Piece{
    public:
        bool isValidMove(int row, int col) const override;
        explicit Knight(char colour);
};

#endif
