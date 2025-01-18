#include "TextObserver.h"
#include "Chessboard.h"
#include "Piece.h"
#include <iostream>
#include <iomanip>
#include "./exceptions/InternalErrorException.h"

TextObserver::TextObserver(const Chessboard* board) : subject{board} {}

void TextObserver::notify() {
    for(size_t i = Chessboard::BOARD_SIZE; i > 0; i--){
        std::cout << i << ' ';
        for(size_t j = 0; j < Chessboard::BOARD_SIZE; j++){
            // i is row, j is column
            const Piece* piece = subject->getState(i - 1, j).first;
            if(piece) std::cout << std::setw(2) << piece->getName();
            else std::cout << std::setw(2) << ((((Chessboard::BOARD_SIZE - i - 2) + j) % 2 == 0) ? ' ' : '_');
        }
        std::cout << std::endl;
    }
    std::cout << "\n abcdefgh" << std::endl;
}
