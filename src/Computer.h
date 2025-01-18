#ifndef COMPUTER_H
#define COMPUTER_H

#include "Player.h"
#include <unordered_map>
#include <vector>

class Chessboard; // Forward declaration

class Computer : public Player {
    int level;
    std::string level1Move(const Chessboard* board);
    std::string level2Move(const Chessboard* board);
    std::string level3Move(const Chessboard* board);
    std::string level4Move(const Chessboard* board);
    static const std::unordered_map<char, int> pieceValues;
    int evaluateBoard(Chessboard *board);
    int EndOfGameScore(Chessboard* board);
    public:
        static const std::unordered_map<char, int> supportedLevels;
        explicit Computer(std::string id, int level);
        std::string getMove(const Chessboard* board);
};

#endif
