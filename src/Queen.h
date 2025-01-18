#ifndef QUEEN_H
#define QUEEN_H

#include "Piece.h"

class Queen : public Piece{
    public:
        bool isValidMove(int row, int col) const override;
        explicit Queen(char colour);
};

#endif
