#include "GraphicsObserver.h"
#include "Chessboard.h"
#include "Piece.h"

#include "piece-imgs2/black_bishop.h"
#include "piece-imgs2/white_bishop.h"
#include "piece-imgs2/black_king.h"
#include "piece-imgs2/white_king.h"
#include "piece-imgs2/black_knight.h"
#include "piece-imgs2/white_knight.h"
#include "piece-imgs2/black_queen.h"
#include "piece-imgs2/white_queen.h"
#include "piece-imgs2/black_pawn.h"
#include "piece-imgs2/white_pawn.h"
#include "piece-imgs2/black_rook.h"
#include "piece-imgs2/white_rook.h"

const std::unordered_map<std::string, Theme> GraphicsObserver::themes = {
    {"standard", {Xwindow::White, Xwindow::Green}},
    {"coral", {Xwindow::coralLight, Xwindow::coralDark}},
    {"dusk", {Xwindow::duskLight, Xwindow::duskDark}},
    {"marine", {Xwindow::marineLight, Xwindow::marineDark}}
};

array_type& GraphicsObserver::getPieceImgArray(char piece){
    switch (piece){
        case 'b': return blackBishop;
        case 'B': return whiteBishop;
        case 'p': return blackPawn;
        case 'P': return whitePawn;
        case 'n': return blackKnight;
        case 'N': return whiteKnight;
        case 'k': return blackKing;
        case 'K': return whiteKing;
        case 'q': return blackQueen;
        case 'Q': return whiteQueen;
        case 'r': return blackRook;
        case 'R': return whiteRook;
    }
    return whitePawn;
}

GraphicsObserver::GraphicsObserver(const Chessboard* board, const Theme& theme) : subject{board}, currentTheme{theme} {
    window = std::make_unique<Xwindow>(GRID_SIZE * Chessboard::BOARD_SIZE, GRID_SIZE * Chessboard::BOARD_SIZE);
    for (size_t row = 0; row < Chessboard::BOARD_SIZE; ++row) {
        for (size_t col = 0; col < Chessboard::BOARD_SIZE; ++col) {
            int squareColor = (((Chessboard::BOARD_SIZE - row - 1) + col) % 2 == 0) ? currentTheme.lightSquareColour : currentTheme.darkSquareColour;
            window->fillRectangle(col * GRID_SIZE, (Chessboard::BOARD_SIZE - row - 1) * GRID_SIZE, GRID_SIZE, GRID_SIZE, squareColor);
        }
    }
}

void GraphicsObserver::notify() {
    for (size_t row = 0; row < Chessboard::BOARD_SIZE; ++row) {
        for (size_t col = 0; col < Chessboard::BOARD_SIZE; ++col) {
            std::pair<const Piece*, const Piece*> pieces = subject->getState(row, col);
            const Piece* oldPiece = pieces.second;
            const Piece* piece = pieces.first;

            int squareColor = (((Chessboard::BOARD_SIZE - row - 1) + col) % 2 == 0) ? currentTheme.lightSquareColour : currentTheme.darkSquareColour;

            if(oldPiece && !piece){
                int squareColor = (((Chessboard::BOARD_SIZE - row - 1) + col) % 2 == 0) ? currentTheme.lightSquareColour : currentTheme.darkSquareColour;
                window->fillRectangle(col * GRID_SIZE, (Chessboard::BOARD_SIZE - row - 1) * GRID_SIZE, GRID_SIZE, GRID_SIZE, squareColor);
            }
            else if(!oldPiece && piece){
                window->drawPiece(col*GRID_SIZE, (Chessboard::BOARD_SIZE - row - 1)*GRID_SIZE, squareColor, getPieceImgArray(piece->getName()));
            }
            else if(oldPiece && piece && *oldPiece != *piece){
                window->drawPiece(col*GRID_SIZE, (Chessboard::BOARD_SIZE - row - 1)*GRID_SIZE, squareColor, getPieceImgArray(piece->getName()));
            }
        }
    }
}

GraphicsObserver::~GraphicsObserver() {}
