#include "Computer.h"
#include "Chessboard.h"
#include "Piece.h"
#include "Pawn.h"
#include "Knight.h"
#include <random>
#include <chrono>
#include "exceptions/InternalErrorException.h"
#include <iostream>

const std::unordered_map<char, int> Computer::supportedLevels = {{'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}};

const std::unordered_map<char, int> Computer::pieceValues = {{'K', 0}, {'P', 1}, {'N', 3}, {'B', 3}, {'R', 5}, {'Q', 9}};

Computer::Computer(std::string id, int level) : Player{id}, level{level} {}

std::string Computer::getMove(const Chessboard* board) {
    if(level == 1){
        return level1Move(board);
    }
    else if(level == 2){
        return level2Move(board);
    }
    else if(level == 3){
        return level3Move(board);
    }
    else if(level == 4){
        return level4Move(board);
    }
    return "";
}

std::string Computer::level1Move(const Chessboard* board){
    std::vector<std::string> validMoves;
    board->getAllMoves(getId(), validMoves);
    if(validMoves.empty()){
        throw InternalErrorException{"Computer generated no moves"};
    }
    // Pick a move at random
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(0, validMoves.size() - 1);
    std::string move = validMoves[distribution(generator)];
    return move;
}

std::string Computer::level2Move(const Chessboard* board){
    std::vector<std::string> validMoves;
    board->getAllMoves(getId(), validMoves);
    std::vector<std::string> preferredMoves;
    for(const std::string &move : validMoves){
        int row, col;

        col = move[3] - 'a';
        row = move[4] - '1';

        // Check if move is capturing
        const Piece* pieceAtDest = board->getState(row, col).first;
        if(pieceAtDest != nullptr && pieceAtDest->getColour() != toupper(getId()[0])){
            preferredMoves.push_back(move);
            continue;
        }

        // Check if move is a check
        Chessboard copy = *board;
        copy.executeMove(move, getId());
        if(copy.isKingInCheck(toupper(getId()[0]) == 'W' ? 'B' : 'W')){
            preferredMoves.push_back(move);
            continue;
        }
        
        // Pawn promotion
        if(move.size() > 5){
            preferredMoves.push_back(move);
            continue;
        }
    }
    std::string move;
    if(preferredMoves.empty()){
        if(validMoves.empty()){
            throw InternalErrorException{"Computer generated no moves"};
        }
        // Pick a random move
        std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> distribution(0, validMoves.size() - 1);
        move = validMoves[distribution(generator)];
    }
    else{
        // Pick a preferred move at random
        std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> distribution(0, preferredMoves.size() - 1);
        move = preferredMoves[distribution(generator)];
    }
    return move;
}

std::string Computer::level3Move(const Chessboard* board){
    std::vector<std::string> validMoves;
    board->getAllMoves(getId(), validMoves);
    std::vector<std::string> prunedValidMoves;
    std::vector<std::string> preferredMoves;
    std::vector<std::string> morePreferredMoves;
    for(const std::string &move : validMoves){
        int row, col;
        col = move[3] - 'a';
        row = move[4] - '1';

        int originalRow, originalCol;
        originalCol = move[0] - 'a';
        originalRow = move[1] - '1'; 

        {
            Chessboard copy = *board;
            if(!copy.isPieceBeingAttacked(col, row, toupper(getId()[0]))){
                copy.isPieceBeingAttacked(col, row, toupper(getId()[0]));
                prunedValidMoves.push_back(move);
            }
        }

        // Check if move avoids capture
        {
            Chessboard copy = *board;
            if(copy.isPieceBeingAttacked(originalCol, originalRow, toupper(getId()[0]))){
                copy.executeMove(move, getId());
                if(!copy.isPieceBeingAttacked(col, row, toupper(getId()[0]))){
                    morePreferredMoves.push_back(move);
                }
            }
        }

        // Check if move is capturing
        const Piece* pieceAtDest = board->getState(row, col).first;
        if(pieceAtDest != nullptr && pieceAtDest->getColour() != toupper(getId()[0])){
            preferredMoves.push_back(move);
            continue;
        }

        // Pawn promotion
        if(move.size() > 5){
            preferredMoves.push_back(move);
            continue;
        }

        // Check if move is a check
        {
            Chessboard copy = *board;        
            copy.executeMove(move, getId());

            // We don't want to give up a piece for a check
            if(copy.isKingInCheck(toupper(getId()[0]) == 'W' ? 'B' : 'W') && !copy.isPieceBeingAttacked(col, row, toupper(getId()[0]))){
                preferredMoves.push_back(move);
                continue;
            }
        }
    }
    if(!prunedValidMoves.empty()) validMoves = prunedValidMoves;
    std::string move;
    if(morePreferredMoves.empty()){
        if(preferredMoves.empty()){
            if(validMoves.empty()){
                throw InternalErrorException{"Computer generated no moves"};
            }
            // Pick a random move
            std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
            std::uniform_int_distribution<int> distribution(0, validMoves.size() - 1);
            move = validMoves[distribution(generator)];
        }
        else{
            // Pick a preferred move at random
            std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
            std::uniform_int_distribution<int> distribution(0, preferredMoves.size() - 1);
            move = preferredMoves[distribution(generator)];
        }
    }
    else{
        // Pick a more preferred move at random
        std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> distribution(0, morePreferredMoves.size() - 1);
        move = morePreferredMoves[distribution(generator)];
    }
    return move;
}

int Computer::EndOfGameScore(Chessboard* board){
    int score = -1;
    std::vector<std::string> validMoves;
    board->getAllMoves(toupper(getId()[0]) == 'W' ? "black" : "white", validMoves);
    if(validMoves.size() == 0){
        // Checkmate if other king in check, stalemate otherwise
        if(board->isKingInCheck(toupper(getId()[0]) == 'W' ? 'B' : 'W')){
            score = 10000;
        }
        else{
            score = 0; 
        }
    }
    // check for dead position
    int numBishopOrKnight = 0;
    bool twoBishopDeadPos = false;
    std::pair<int, int> tmpBishopPos = {-1, -1};
    for(size_t i = 0; i < Chessboard::BOARD_SIZE; i++){
        for(size_t j = 0; j < Chessboard::BOARD_SIZE; j++){
            if(board->board[i][j] != nullptr && (toupper(board->board[i][j]->getName()) == 'B' || toupper(board->board[i][j]->getName()) == 'N')){
                if(tmpBishopPos.first == -1) tmpBishopPos = std::pair<int, int>(i, j);
                else if(((Chessboard::BOARD_SIZE - i - 1) + j) % 2 == (((Chessboard::BOARD_SIZE - tmpBishopPos.first - 1) + tmpBishopPos.second) % 2)){
                    // two bishops on same colour diagonal
                    twoBishopDeadPos = true;
                }
                numBishopOrKnight++;
            }
        }
    }
    if(numBishopOrKnight == 1 || (numBishopOrKnight == 2 && twoBishopDeadPos) || numBishopOrKnight == 0){
        score = 0;
    }

    //Avoid 50 move draw
    if(board->fiftyMoveDrawCount >= 49){
        score = 0;
    }
    return score;
}

int Computer::evaluateBoard(Chessboard *board){
    int score = 0;
    for(size_t i = 0; i < Chessboard::BOARD_SIZE; i++){
        for(size_t j = 0; j < Chessboard::BOARD_SIZE; j++){
            const Piece *piece = board->getState(i, j).first;
            if(piece){
                if(piece->getColour() == toupper(getId()[0])){
                    // Same colour
                    if(!board->isPieceBeingAttacked(j, i, piece->getColour())){
                        score += pieceValues.at(toupper(piece->getName()));
                    }
                    else{
                        score -= pieceValues.at(toupper(piece->getName()));
                    }
                }
                else{
                    // opposite colour
                    if(!board->isPieceBeingAttacked(j, i, piece->getColour())){
                        score -= pieceValues.at(toupper(piece->getName()));
                    }
                    else{
                        score += pieceValues.at(toupper(piece->getName()));
                    }
                }
            }
        }
    }
    int eogScore = EndOfGameScore(board);
    if(eogScore >= 0){
        score = eogScore;
    }
    return score;
}

std::string Computer::level4Move(const Chessboard* board){
    std::vector<std::string> validMoves;
    board->getAllMoves(getId(), validMoves);
    if(validMoves.empty()){
        throw InternalErrorException{"Computer generated no moves"};
    }
    int maxScore = -1000000;
    std::string bestMove = "";
    for(std::string move : validMoves){
        Chessboard copy = *board;
        copy.executeMove(move, getId());
        int score = evaluateBoard(&copy);
        if(score > maxScore){
            maxScore = score;
            bestMove = move;
        }
    }
    return bestMove;
}
