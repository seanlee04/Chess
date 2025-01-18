#ifndef ROOK_H
#define ROOK_H

#include "Piece.h"

class Rook : public Piece{
    bool isMoved;
    public:
        bool isValidMove(int row, int col) const override;
        explicit Rook(char colour);
        bool hasMoved() const;
        void moved();
};

#endif
