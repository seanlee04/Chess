#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <iostream>
#include <memory>
#include "Chessboard.h"
#include "Player.h"

class GameManager{
    bool inSetupMode;
    std::unique_ptr<Player> player1;
    std::unique_ptr<Player> player2;
    int player1Score;
    int player2Score;
    Player* currentPlayer;
    bool currentlyInGame;
    Chessboard chessboard;
    public:
        GameManager();
        void startGame(std::string player1Info, std::string player2Info);
        void forfeitGame();
        void nextMove(std::string move);
        void printSeriesScore(std::ostream &) const;
        void setSetupMode(bool);
        void runSetupCommand(std::string);
        void attachObserver(std::unique_ptr<Observer> o);
        void detachObserver(std::unique_ptr<Observer> o);
        const Chessboard* getBoard() const; 
};

#endif
