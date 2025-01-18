#ifndef PIECE_H
#define PIECE_H

#include <string>
#include <map>

class Piece{
    char name;
    char colour;
    public:
        Piece(char p, char c);
        virtual bool isValidMove(int row, int col) const = 0;
        virtual char getName() const;
        virtual char getColour() const;
        virtual ~Piece() = default;
        virtual bool operator==(const Piece &other) const;
};

#endif
