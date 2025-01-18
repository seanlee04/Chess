#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "Subject.h"
#include <string>
#include <utility>
#include <set>
#include <unordered_set>
#include <memory>
#include "Computer.h"

class Chessboard : public Subject{
    public:
        inline static const size_t BOARD_SIZE = 8;
        Chessboard();
        Chessboard(const Chessboard &other);
        void addPiece(std::string cmd);
        void removePiece(std::string cmd);
        void setColour(std::string cmd);
        bool executeMove(std::string cmd, std::string playerId);
        std::string getWinner();
        bool validChessboard();
        std::pair<const Piece*, const Piece*> getState(size_t row, size_t col) const override;
        void clearChessboard();
        void initChessboard();
        void getValidPlayerIds(std::string (&ids)[2]);
        ~Chessboard();
    private:
        static const std::unordered_set<char> validWhitePieceInputs;
        static const std::unordered_set<char> validBlackPieceInputs;
        static const std::unordered_set<char> gridNumbers;
        static const std::unordered_set<char> gridLetters;
        static const std::unordered_set<char> promotedWhitePieces;
        static const std::unordered_set<char> promotedBlackPieces;
        bool validPiece(std::string piece);
        bool validGridPosition(std::string position);
        bool isBlackPiece(std::string piece);
        bool isWhitePiece(std::string piece);
        bool validPromotionPiece(std::string piece, char color);
        bool isBlackPromotionPiece(std::string piece);
        bool isWhitePromotionPiece(std::string piece);
        std::unique_ptr<Piece> createPiece(std::string piece, char colour);
        void makeMove(int startRow, int startCol, int endRow, int endCol, bool enPassant = false);
        bool isKingInCheck(char piece);
        bool isPieceBeingAttacked(int x, int y, char colour);
        bool isStraightClear(int startRow, int startCol, int endRow, int endCol);
        bool isDiagonalClear(int startRow, int startCol, int endRow, int endCol);
        void getAllMoves(std::string playerId, std::vector<std::string> &validMoves) const;
        bool isTemporary;
        std::unique_ptr<Piece> board[BOARD_SIZE][BOARD_SIZE];
        std::unique_ptr<Piece> prevBoard[BOARD_SIZE][BOARD_SIZE];
        std::string winner;
        std::string validPlayerIds[2] = {"White", "Black"};
        void updateBackup();
        int fiftyMoveDrawCount;
        friend class Computer;
};

#endif
