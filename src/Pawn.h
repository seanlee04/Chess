#ifndef PAWN_H
#define PAWN_H

#include "Piece.h"

class Pawn : public Piece{
    bool enPassantEnabled;
    public:
        bool isValidMove(int row, int col) const override;
        explicit Pawn(char colour);
        bool enPassantAllowed() const;
        void setEnPassant(bool value);
};

#endif
