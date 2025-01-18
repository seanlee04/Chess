#include <iostream>
#include <string>
#include <memory>

#include "./exceptions/InternalErrorException.h"
#include "./exceptions/InvalidInputException.h"

#include "GameManager.h"
#include "TextObserver.h"
#include "GraphicsObserver.h"

#include <thread>
#include <chrono>

int main(){

    std::string command;
    
    // Initialize GameManager
    GameManager gameManager = GameManager{};
    std::unique_ptr<TextObserver> textObserver = std::make_unique<TextObserver>(gameManager.getBoard());
    
    std::string themeName = "standard"; // Default theme
    std::cout << "Enter theme (standard, coral, marine): ";
    std::cin >> themeName;
    if (GraphicsObserver::themes.find(themeName) == GraphicsObserver::themes.end()) {
        std::cout << "Invalid theme. Using default (standard)." << std::endl;
        themeName = "standard";
    }    
    
    std::unique_ptr<GraphicsObserver> graphicsObserver = std::make_unique<GraphicsObserver>(gameManager.getBoard(), GraphicsObserver::themes.at(themeName));
    gameManager.attachObserver(std::move(textObserver));
    gameManager.attachObserver(std::move(graphicsObserver));

    // Read commands from standard input
    while(std::cin >> command){
        try {
            if(command == "game"){
                // Start game
                std::string player1, player2;
                // Get player information
                std::cin >> player1;
                std::cin >> player2;
                gameManager.startGame(player1, player2);
            }
            else if(command == "resign"){
                // Current player forfeits game
                gameManager.forfeitGame();
            }
            else if (command == "move"){
                std::string move;
                // Get rest of line containing information on move
                std::getline(std::cin, move);
                gameManager.nextMove(move);
            }
            else if(command == "setup"){
                // Enter setup mode
                gameManager.setSetupMode(true);
            }
            else if(command == "+" || command == "-" || command == "="){
                // If we encounter any of the setup commands
                // Pass the entire line to the game manager
                std::string setupCmd;
                std::getline(std::cin, setupCmd);
                gameManager.runSetupCommand(command + setupCmd);
            }
            else if(command == "done"){
                // Exit setup mode
                gameManager.runSetupCommand(command);
            }
        }
        catch(const InternalErrorException& e) { 
            std::cout << e.what() << std::endl;
            break;
        }
        catch(const InvalidInputException& e) {
            std::cout << e.what() << std::endl;
            // break;
        }
        catch(...) {
            std::cout << "An unknown exception occurred." << std::endl;
            break;
        }
    }
    
    if(std::cin.eof()){
        // Ctrl-D's been pressed
        // print series score
        gameManager.printSeriesScore(std::cout);
    }

    return 0;
}
