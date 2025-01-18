#ifndef KING_H
#define KING_H

#include "Piece.h"
#include <utility>
#include <set>

class King : public Piece{
    bool isMoved;
    public:
        bool isValidMove(int row, int col) const override;
        explicit King(char colour);
        bool hasMoved() const;
        void moved();
};

#endif
