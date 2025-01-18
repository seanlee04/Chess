#include "GameManager.h"
#include "Human.h"
#include "./exceptions/InternalErrorException.h"
#include "./exceptions/InvalidInputException.h"
#include "Computer.h"

GameManager::GameManager() : inSetupMode{false}, player1Score{0}, player2Score{0}, currentlyInGame{false} {}

void GameManager::startGame(std::string player1Info, std::string player2Info){
    if(inSetupMode){
        throw InvalidInputException{"Can't start game while in setup mode."};
    }
    const std::string computerPrefix = "computer";
    std::string validIds[2];
    chessboard.getValidPlayerIds(validIds);
    
    // white player
    if(player1Info == "human")
    {
        player1 = std::make_unique<Human>(validIds[0]);
    }
    else if(player1Info.substr(0, computerPrefix.size()) == "computer"){
        if(player1Info.length() == computerPrefix.size() + 1 && Computer::supportedLevels.contains(player1Info[computerPrefix.size()])){
            player1 = std::make_unique<Computer>(validIds[0], Computer::supportedLevels.at(player1Info[computerPrefix.size()]));
        }
        else{
            throw InvalidInputException{"Invalid computer level: player1"};
        }
    }
    else{
        throw InvalidInputException{"Invalid input: player1"};
    }

    // black player
    if(player2Info == "human")
    {
        player2 = std::make_unique<Human>(validIds[1]);
    }
    else if(player2Info.substr(0, computerPrefix.size()) == "computer"){
        if(player2Info.length() == computerPrefix.size() + 1 && Computer::supportedLevels.contains(player2Info[computerPrefix.size()])){
            player2 = std::make_unique<Computer>(validIds[1], Computer::supportedLevels.at(player2Info[computerPrefix.size()]));
        }
        else{
            // Move player into temp so when temp goes out of scope, gets destructed
            std::unique_ptr<Player> temp = std::move(player1);
            throw InvalidInputException{"Invalid computer level: player2"};
        }    
    }
    else{
        // Move player into temp so when temp goes out of scope, gets destructed
        std::unique_ptr<Player> temp = std::move(player1);
        throw InvalidInputException{"Invalid input: player2"};
    }

    currentPlayer = player1.get();
    currentlyInGame = true;
    chessboard.notifyObservers();
}

void GameManager::forfeitGame(){
    if(currentPlayer == player1.get()){
        player2Score+=2;
    }
    else{
        player1Score+=2;
    }
    std::cout << currentPlayer->getId() << " wins!" << std::endl;

    chessboard.initChessboard();
    currentlyInGame = false;
}

void GameManager::nextMove(std::string move){
    if(!currentlyInGame){
        throw InvalidInputException{"No game currently running!"};
    }
    Computer* computerPtr = dynamic_cast<Computer*>(currentPlayer);
    bool gameDone;
    if(computerPtr){
        // Query computer for move
        std::string compMove = computerPtr->getMove(getBoard());
        gameDone = chessboard.executeMove(compMove, currentPlayer->getId());
    }
    else{
        // Relay move directly to ChessBoard
        gameDone = chessboard.executeMove(move, currentPlayer->getId());
    }
    if(gameDone){
        if(chessboard.getWinner() == player1->getId()){
            player1Score += 2;
        }
        else if(chessboard.getWinner() == player2->getId()){
            player2Score += 2;
        }
        else{
            player1Score += 1;
            player2Score += 1;
        }
        currentlyInGame = false;
    }
    currentPlayer = (currentPlayer == player1.get()) ? player2.get() : player1.get();
}

void GameManager::printSeriesScore(std::ostream &o) const{
    o << "Final Score: " << std::endl;
    o << (player1 ? player1->getId() : "White") << ": " << (player1Score / 2) << (player1Score % 2 ? ".5" : "") << std::endl 
        << (player2 ? player2->getId() : "Black") << ": " << (player2Score / 2) << (player2Score % 2 ? ".5" : "") << std::endl;
}

void GameManager::setSetupMode(bool enableSetupMode){
    if(!currentlyInGame && enableSetupMode) {
        inSetupMode = enableSetupMode;
        chessboard.clearChessboard();
        chessboard.notifyObservers();
    } else if(!enableSetupMode) {
        inSetupMode = enableSetupMode;
    } else {
        throw InvalidInputException("Invalid Input: Can't enter setup mode while in game");
    }
}

void GameManager::runSetupCommand(std::string cmd){
    if(inSetupMode)
    {
        if(cmd[0] == '+'){
            chessboard.addPiece(cmd.substr(1));
        }
        else if(cmd[0] == '-'){
            chessboard.removePiece(cmd.substr(1));
        }
        else if(cmd[0] == '='){
            chessboard.setColour(cmd.substr(1));
        }
        else if (cmd == "done"){
            // stay in setup mode if board is not valid
            if(!chessboard.validChessboard()){
                chessboard.notifyObservers();
                throw InvalidInputException{"Invalid Input: board is not valid, can't exit setup mode"};
            }
            setSetupMode(false);
        }
        else{
            // Should never reach this since game manager directly passes command
            throw InternalErrorException{"Internal Error: Undefined Behavior in GameManager"};
        }
        chessboard.notifyObservers();
    }
    else {
        throw InvalidInputException{"Invalid Input: Not in setup mode"};
    }
}

void GameManager::attachObserver(std::unique_ptr<Observer> o){
    chessboard.attach(std::move(o));
}

void GameManager::detachObserver(std::unique_ptr<Observer> o){
    chessboard.detach(std::move(o));
}

const Chessboard* GameManager::getBoard() const{
    return &chessboard;
}
