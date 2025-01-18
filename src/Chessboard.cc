#include "Chessboard.h"

#include "Piece.h"
#include "King.h"
#include "Queen.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"
#include "Pawn.h"
// #include "BlankPiece.h"

#include "./exceptions/InternalErrorException.h"
#include "./exceptions/InvalidInputException.h"

#include <sstream>

const std::unordered_set<char> Chessboard::validWhitePieceInputs = { 'K', 'Q', 'R', 'B', 'N', 'P' };
const std::unordered_set<char> Chessboard::validBlackPieceInputs = { 'k', 'q', 'r', 'b', 'n', 'p'};
const std::unordered_set<char> Chessboard::gridNumbers = { '1', '2', '3', '4', '5', '6', '7', '8' };
const std::unordered_set<char> Chessboard::gridLetters = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
const std::unordered_set<char> Chessboard::promotedWhitePieces = { 'Q', 'R', 'B', 'N' };
const std::unordered_set<char> Chessboard::promotedBlackPieces = { 'q', 'r', 'b', 'n' };

Chessboard::Chessboard() : isTemporary{false}, board{}, prevBoard{}, fiftyMoveDrawCount(0) {
    initChessboard();
}

Chessboard::Chessboard(const Chessboard &other) : isTemporary{true}, board{}, prevBoard{}, fiftyMoveDrawCount{other.fiftyMoveDrawCount} {
    for(size_t i = 0; i < BOARD_SIZE; i++){
        for(size_t j = 0; j < BOARD_SIZE; j++){
            if(other.board[i][j]){
                board[i][j] = createPiece(std::string{other.board[i][j]->getName()}, other.board[i][j]->getColour());
                // set any flags on this piece as true
                if(toupper(board[i][j]->getName()) == 'P'){
                    Pawn* pawn1 = dynamic_cast<Pawn*>(board[i][j].get());
                    Pawn* pawn2 = dynamic_cast<Pawn*>(other.board[i][j].get());
                    pawn1->setEnPassant(pawn2->enPassantAllowed());
                }
                else if(toupper(board[i][j]->getName()) == 'K'){
                    King* king1 = dynamic_cast<King*>(board[i][j].get());
                    King* king2 = dynamic_cast<King*>(other.board[i][j].get());
                    if(king2->hasMoved()) king1->moved();
                }
                else if(toupper(board[i][j]->getName()) == 'R'){
                    Rook* rook1 = dynamic_cast<Rook*>(board[i][j].get());
                    Rook* rook2 = dynamic_cast<Rook*>(other.board[i][j].get());
                    if(rook2->hasMoved()) rook1->moved();
                }
            }
        }
    }
}

void Chessboard::addPiece(std::string cmd){
    std::istringstream strm{cmd};
    std::string piece;
    strm >> piece;
    std::string position;
    strm >> position;
    
    if(!validPiece(piece)){
        throw InvalidInputException{"Invalid input: piece"};
    }

    // exceptions should propegate all the way up to user
    if(!validGridPosition(position)){
        throw InvalidInputException{"Invalid input: position"};
    }

    updateBackup();

    int row, col;
    col = position[0] - 'a';
    row = position[1] - '1';
    if(board[row][col]){
        std::unique_ptr<Piece> tmp;
        tmp = std::move(board[row][col]);
    }
    
    if(isWhitePiece(piece)){
        board[row][col] = createPiece(piece, 'W');
    }
    if(isBlackPiece(piece)){
        board[row][col] = createPiece(piece, 'B');
    }
}

void Chessboard::removePiece(std::string cmd){
    std::istringstream strm{cmd};
    std::string position;
    strm >> position;

    if(!validGridPosition(position)){
        throw InvalidInputException{"Invalid input: position"};
    }

    updateBackup();

    int row, col;
    col = position[0] - 'a';
    row = position[1] - '1';
    if(board[row][col]){
        std::unique_ptr<Piece> tmp;
        tmp = std::move(board[row][col]);
    }
}

void Chessboard::getValidPlayerIds(std::string (&ids)[2]){
    ids[0] = validPlayerIds[0];
    ids[1] = validPlayerIds[1];
}

void Chessboard::setColour(std::string cmd){
    // Normalize input
    for(char &c : cmd){
        // Capitalize first letter
        if(c != ' '){
            c = toupper(c);
            break;
        }
    }
    if(cmd.find(validPlayerIds[1]) != std::string::npos){
        std::swap(validPlayerIds[0], validPlayerIds[1]);
    }
    else if(cmd.find(validPlayerIds[0]) == std::string::npos){
        throw InvalidInputException{"Invalid colour specified. "};
    }
}

void Chessboard::getAllMoves(std::string playerId, std::vector<std::string> &validMoves) const{
    for(size_t i = 0; i < Chessboard::BOARD_SIZE; i++){
        for(size_t j = 0; j < Chessboard::BOARD_SIZE; j++){
            const Piece* piece = getState(i, j).first;
            if(piece != nullptr && toupper(playerId[0]) == piece->getColour()){
                // Only use non blank pieces and pieces matching players colour
                char pieceLoc[2]; pieceLoc[0] = j + 'a'; pieceLoc[1] = i + '1';
                switch (piece->getName()){
                    case 'P':
                    {
                        //White pawn
                        const int moves[4][2] = {{0, 1}, {0, 2}, {1, 1}, {-1, 1}};
                        for(int i = 0; i < 4; i++){
                            std::string move = std::string{pieceLoc[0]} + std::string{pieceLoc[1]} + std::string{' '} + 
                                std::string{static_cast<char>(pieceLoc[0] + moves[i][0])} + 
                                std::string{static_cast<char>(pieceLoc[1] + moves[i][1])};
                            if(pieceLoc[1] + moves[i][1] == '8'){
                                // Pawn promotion
                                move += " Q";
                            }
                            Chessboard copy = *this;
                            try{
                                copy.executeMove(move, playerId);
                                validMoves.push_back(move);
                            }
                            catch(const InvalidInputException&){
                                continue;
                            }
                        }
                        break;
                    }
                    case 'p':
                    {
                        //Black pawn
                        const int moves[4][2] = {{0, -1}, {0, -2}, {1, -1}, {-1, -1}};
                        for(int i = 0; i < 4; i++){
                            std::string move = std::string{pieceLoc[0]} + std::string{pieceLoc[1]} + std::string{' '} + 
                                std::string{static_cast<char>(pieceLoc[0] + moves[i][0])} + 
                                std::string{static_cast<char>(pieceLoc[1] + moves[i][1])};
                            if(pieceLoc[1] + moves[i][1] == '1'){
                                // Pawn promotion
                                move += " q";
                            }
                            Chessboard copy = *this;
                            try{
                                copy.executeMove(move, playerId);
                                validMoves.push_back(move);
                            }
                            catch(const InvalidInputException&){
                                continue;
                            }
                        }
                        break;
                    }
                    case 'N': case 'n':
                    {
                        // Knight
                        const int moves[8][2] = {{1, 2}, {-1, 2}, {1, -2}, {-1, -2}, {2, 1}, {-2, 1}, {2, -1}, {-2, -1}};
                        for(int i = 0; i < 8; i++){
                            std::string move = std::string{pieceLoc[0]} + std::string{pieceLoc[1]} + std::string{' '} + 
                                std::string{static_cast<char>(pieceLoc[0] + moves[i][0])} + 
                                std::string{static_cast<char>(pieceLoc[1] + moves[i][1])};
                            Chessboard copy = *this;
                            try{
                                copy.executeMove(move, playerId);
                                validMoves.push_back(move);
                            }
                            catch(const InvalidInputException&){
                                continue;
                            }
                        }
                        break;
                    }
                    case 'B': case 'b':
                    {
                        // Bishop
                        for(int i = 1; i <= 7; i++){
                            for(int j = -1; j <= 1; j+=2){
                                for(int k = -1; k <= 1; k+=2){
                                    std::string move = std::string{pieceLoc[0]} + std::string{pieceLoc[1]} + std::string{' '} + 
                                        std::string{static_cast<char>(pieceLoc[0] + j*i)} + 
                                        std::string{static_cast<char>(pieceLoc[1] + k*i)};
                                    Chessboard copy = *this;
                                    try{
                                        copy.executeMove(move, playerId);
                                        validMoves.push_back(move);
                                    }
                                    catch(const InvalidInputException&){
                                        continue;
                                    }
                                }
                            }
                        }
                        break;
                    }
                    case 'R': case 'r':
                    {
                        // Rook
                        for(int i = 0; i <= 1; i++){
                            for(int j = -7; j <= 7; j++){
                                std::string move = std::string{pieceLoc[0]} + std::string{pieceLoc[1]} + std::string{' '} + 
                                    std::string{static_cast<char>(pieceLoc[0] + (i ? 0 : 1)*j)} + 
                                    std::string{static_cast<char>(pieceLoc[1] + (i ? 1 : 0)*j)};
                                Chessboard copy = *this;
                                try{
                                    copy.executeMove(move, playerId);
                                    validMoves.push_back(move);
                                }
                                catch(const InvalidInputException&){
                                    continue;
                                }
                            }
                        }
                        break;
                    }
                    case 'Q': case 'q':
                    {
                        // Queen
                        for(int i = 1; i <= 7; i++){
                            for(int j = -1; j <= 1; j+=2){
                                for(int k = -1; k <= 1; k+=2){
                                    std::string move = std::string{pieceLoc[0]} + std::string{pieceLoc[1]} + std::string{' '} + 
                                        std::string{static_cast<char>(pieceLoc[0] + j*i)} + 
                                        std::string{static_cast<char>(pieceLoc[1] + k*i)};
                                    Chessboard copy = *this;
                                    try{
                                        copy.executeMove(move, playerId);
                                        validMoves.push_back(move);
                                    }
                                    catch(const InvalidInputException&){
                                        continue;
                                    }
                                }
                            }
                        }
                        for(int i = 0; i <= 1; i++){
                            for(int j = -7; j <= 7; j++){
                                std::string move = std::string{pieceLoc[0]} + std::string{pieceLoc[1]} + std::string{' '} + 
                                    std::string{static_cast<char>(pieceLoc[0] + (i ? 0 : 1)*j)} + 
                                    std::string{static_cast<char>(pieceLoc[1] + (i ? 1 : 0)*j)};
                                Chessboard copy = *this;
                                try{
                                    copy.executeMove(move, playerId);
                                    validMoves.push_back(move);
                                }
                                catch(const InvalidInputException&){
                                    continue;
                                }
                            }
                        }
                        break;
                    }
                    case 'K': case 'k': 
                    {
                        // King
                        const int moves[10][2] = {{0, 1}, {0, -1}, {1, 0}, {1, 1}, {1, -1}, {-1, 0}, {-1, 1}, {-1, -1}, {2, 0}, {-2, 0}};
                        for(int i = 0; i < 10; i++){
                            std::string move = std::string{pieceLoc[0]} + std::string{pieceLoc[1]} + std::string{' '} + 
                                std::string{static_cast<char>(pieceLoc[0] + moves[i][0])} + 
                                std::string{static_cast<char>(pieceLoc[1] + moves[i][1])};
                            Chessboard copy = *this;
                            try{
                                copy.executeMove(move, playerId);
                                validMoves.push_back(move);
                            }
                            catch(const InvalidInputException&){
                                continue;
                            }
                        }
                        break;
                    }  
                }
            }
        }
    }
}

void Chessboard::updateBackup(){
    // Store board before modification
    for(size_t i = 0; i < BOARD_SIZE; i++){
        for(size_t j = 0; j < BOARD_SIZE; j++){
            if(prevBoard[i][j]){
                std::unique_ptr<Piece> tmp;
                tmp = std::move(prevBoard[i][j]);
            }
            if(board[i][j]){
                prevBoard[i][j] = createPiece(std::string{board[i][j]->getName()}, board[i][j]->getColour());
            }
        }
    }
}

bool Chessboard::executeMove(std::string cmd, std::string playerId){
    // validate command input and parse the move
    std::istringstream strm{cmd};
    std::string position1;
    strm >> position1;
    std::string position2;
    strm >> position2;

    if(!validGridPosition(position1)){
        throw InvalidInputException{"Invalid input: position1"};
    }
    if(!validGridPosition(position2)){
        throw InvalidInputException{"Invalid input: position2"};
    }

    int row1, col1, row2, col2;
    col1 = position1[0] - 'a';
    row1 = position1[1] - '1';

    col2 = position2[0] - 'a';
    row2 = position2[1] - '1';
   
    // validate the move
    // check if piece exists at starting position
    if(board[row1][col1] == nullptr) {
        throw InvalidInputException{"Invalid input: No piece exists at starting position"};
    }

    std::string piece1name{board[row1][col1]->getName()};
    char piece1Colour = board[row1][col1]->getColour();

    // check if player is moving one of their pieces
    if(playerId == "White" && isBlackPiece(piece1name)) {
        throw InvalidInputException{"Invalid input: White tried to move a black piece"};
    }

    if(playerId == "Black" && isWhitePiece(piece1name)) {
        throw InvalidInputException{"Invalid input: Black tried to move a white piece"};
    }

    int relativeMoveY = row2 - row1;
    int relativeMoveX = col2 - col1;

    if(playerId == "Black") {
        relativeMoveY *= -1;
    }

    if(!board[row1][col1]->isValidMove(relativeMoveY, relativeMoveX)) {
        throw InvalidInputException{"Invalid input: That piece cannot move there"};
    }

    // checks for castle
    bool kingWasInCheck = isKingInCheck(piece1Colour);
    bool castleSuccessful = false;
    // checks for en passant
    bool enPassantSuccessful = false;
    if(piece1name == "k" || piece1name == "K") {
        if((relativeMoveX == -2 || relativeMoveX == 2) && relativeMoveY == 0) { // castle conditions
            // If king is not in right spot we can immediately say no castle
            if(col1 != 4 || row1 != 0){
                throw InvalidInputException{"Invalid input: King not in right spot"};
            }

            King* kingPtr = dynamic_cast<King*>(board[row1][col1].get());
            Rook* rookPtr = dynamic_cast<Rook*>(board[row2][col1 + relativeMoveX + (relativeMoveX == 2 ? 1 : -2)].get());

            if(!rookPtr){
                throw InvalidInputException{"Invalid input: Cannot castle when no rook exists"};
            }

            if(kingPtr->hasMoved()) {
                throw InvalidInputException{"Invalid input: Cannot castle when king has already moved"};
            }
            if(rookPtr->hasMoved()) {
                throw InvalidInputException{"Invalid input: Cannot castle when rook has already moved"};
            }

            if(kingWasInCheck) {
                throw InvalidInputException{"Invalid input: Cannot castle when king is in check"};
            }
            if(relativeMoveX == -2) {
                for(int i = -1; i>-4; i--) {
                    if(isPieceBeingAttacked(col1 + i, row1, piece1Colour)) {
                        throw InvalidInputException{"Invalid input: Cannot castle since the squares in between are under attack"};
                    }
                }
            }
            else if (relativeMoveX == 2) {
                for(int i = 1; i<3; i++) {
                    if(isPieceBeingAttacked(col1+i, row1, piece1Colour)) {
                        throw InvalidInputException{"Invalid input: Cannot castle since the squares in between are under attack"};
                    }
                }
            }
            castleSuccessful = true;
        }
    }
    else if(piece1name == "p" || piece1name == "P") {
        bool enPassantable = false;
        // pawn trying to capture/en passant
        if(abs(relativeMoveX) == 1 && abs(relativeMoveY) == 1) {
            // en passant
            if(board[row2][col2] == nullptr) {
                if(board[row1][col2] == nullptr) {
                    throw InvalidInputException{"Invalid input: No piece to en passant"};
                }
                char targetColour = board[row1][col2]->getColour();

                if(targetColour == 'W' && playerId == "White") {
                    throw InvalidInputException{"Invalid input: Cant en passant own piece"};
                }
                else if(targetColour == 'B' && playerId == "Black") {
                    throw InvalidInputException{"Invalid input: Cant en passant own piece"};
                }

                char enPassantPiece = board[row1][col2]->getName();

                if(enPassantPiece == 'P' || enPassantPiece == 'p') {
                    Pawn* pawnPtr = dynamic_cast<Pawn*>(board[row1][col2].get());
                    if(!pawnPtr->enPassantAllowed()) {
                        throw InvalidInputException{"Invalid input: Cant en passant that pawn"};
                    }
                    else {
                        enPassantSuccessful = true;
                    }
                }
                else {
                    throw InvalidInputException{"Invalid input: Cant en passant non-pawns"};
                }
            }
            enPassantable = false;
        }
        // if its the pawns first move and it moves 2
        else if(abs(relativeMoveY) == 2 && relativeMoveX == 0) {
            // can only move forward 2 on first turn
            if ((piece1Colour == 'W' && row1 != 1)
                || (piece1Colour == 'B' && row1 != 6)) {
                throw InvalidInputException{"Invalid input: Cannot move pawn 2 spaces forward unless it has yet to move"};
            }
            // also cannot capture
            if(board[row2][col2] != nullptr) {
                throw InvalidInputException{"Invalid input: Pawn cannot capture without a diagonal move"};
            }
            enPassantable = true;
        }
        // normal pawn move (can't capture)
        else {
            if(board[row2][col2] != nullptr) {
                throw InvalidInputException{"Invalid input: Pawn cannot capture without a diagonal move"};
            }
            enPassantable = false;
        }
        Pawn* pawnPtr = dynamic_cast<Pawn*>(board[row1][col1].get());
        pawnPtr->setEnPassant(enPassantable);
    }

    // if a piece exists at target, ensure that piece is opposite colour (capture case)
    if(board[row2][col2] != nullptr) {
        if(board[row2][col2]->getColour() == piece1Colour) {
            throw InvalidInputException{"Invalid input: Cannot capture own piece"};
        }
        // ensure we are not capturing a king
        if(toupper(board[row2][col2]->getName()) == 'K'){
            throw InvalidInputException{"Invalid input: Cannot capture king"};
        }
    }

    // check if path to the target is clear
    switch(piece1name[0]) {
        case 'Q': case 'q': 
            if(col1 == col2 || row1 == row2){
                // Vertical queen movement
                if (!isStraightClear(col1, row1, col2, row2)) {
                    throw InvalidInputException{"Invalid input: Illegal move"};
                }
            }
            else{
                // Diagonal queen movement
                if (!isDiagonalClear(col1, row1, col2, row2)) {
                throw InvalidInputException{"Invalid input: Illegal move"};
                }
            }
            break;
        case 'R': case 'r': 
            if(!isStraightClear(col1, row1, col2, row2)) {
                throw InvalidInputException{"Invalid input: Illegal move"};
            }
            break;
        case 'B': case 'b': 
            if(!isDiagonalClear(col1, row1, col2, row2)) {
                throw InvalidInputException{"Invalid input: Illegal move"};
            }
            break;
        case 'P': case 'p': 
            if (!isStraightClear(col1, row1, col2, row2)) {
                throw InvalidInputException{"Invalid input: Illegal move"};
            }
            break;
        case 'N': case 'n': 
        case 'K': case 'k':
            break;
        default:
            throw InternalErrorException{"Internal Error: isPieceBeingAttacked Broken"};
    }

    // make the move (copy constructor chessboard->make move on that board, 
    // king is in check, if not, make move on real board, if it is, move is illegal)
    Chessboard copiedBoard = *this;
    copiedBoard.makeMove(row1, col1, row2, col2, enPassantSuccessful);
    if (copiedBoard.isKingInCheck(piece1Colour)) {
        throw InvalidInputException{"Invalid input: Move leaves king in check"};
    }

    if(castleSuccessful){
        // Update flags on king and rook
        King* kingPtr = dynamic_cast<King*>(board[row1][col1].get());
        Rook* rookPtr = dynamic_cast<Rook*>(board[row2][col1 + relativeMoveX + (relativeMoveX == 2 ? 1 : -2)].get());
        kingPtr->moved();
        rookPtr->moved();
    }

    updateBackup();

    makeMove(row1, col1, row2, col2, enPassantSuccessful);
    if(castleSuccessful){
        makeMove(row1, (relativeMoveX == 2) ? 7 : 0, row2, (relativeMoveX == 2) ? 5 : 3);
    }
    
    // update state of pieces (hasMoved flags, and promotions)
    if((piece1name == "P" || piece1name == "p") && (row2 == 7 || row2 == 0)) {
        std::string newPiece;
        strm >> newPiece;

        if(!isTemporary){
            std::cout << "New piece: " << newPiece << std::endl;
            std::cout << "col: " << piece1Colour << std::endl;
        }
        if(validPromotionPiece(newPiece, piece1Colour)) {
            std::unique_ptr<Piece> tmp;
            tmp = std::move(board[row2][col2]);
            board[row2][col2] = createPiece(newPiece, piece1Colour);
        }
        else {
            throw InvalidInputException{"Invalid Input: That is an invalid promotion piece"};
        }
    }
    else if(piece1name == "R" || piece1name == "r") {
        Rook* rookPtr = dynamic_cast<Rook*>(board[row2][col2].get());
        rookPtr->moved();
    }
    else if(piece1name == "K" || piece1name == "k" ) {
        King* kingPtr = dynamic_cast<King*>(board[row2][col2].get());
        kingPtr->moved();
    }

    // check for checkmate/stalemate (also add logic for endgame-> CALL initChessboard!!!!!)
    if(!isTemporary){
        std::vector<std::string> validMoves;
        getAllMoves(toupper(playerId[0]) == 'W' ? "Black" : "White", validMoves);
        if(validMoves.size() == 0){
            // Checkmate if king in check, stalemate otherwise
            if(isKingInCheck(toupper(playerId[0]) == 'W' ? 'B' : 'W')){
                winner = playerId;
                std::cout << "Checkmate! " << playerId << " wins!" << std::endl;   
            }
            else{
                winner = "tie";
                std::cout << "Stalemate! " << std::endl;  
            }
            notifyObservers();
            initChessboard();
            return true;
        }
        // check for dead position
        int numBishopOrKnight = 0;
        bool twoBishopDeadPos = false;
        std::pair<int, int> tmpBishopPos = {-1, -1};
        for(size_t i = 0; i < BOARD_SIZE; i++){
            for(size_t j = 0; j < BOARD_SIZE; j++){
                if(board[i][j] != nullptr){
                    if((toupper(board[i][j]->getName()) == 'B' || toupper(board[i][j]->getName()) == 'N')){
                        if(tmpBishopPos.first == -1) tmpBishopPos = std::pair<int, int>(i, j);
                        else if(((Chessboard::BOARD_SIZE - i - 1) + j) % 2 == (((Chessboard::BOARD_SIZE - tmpBishopPos.first - 1) + tmpBishopPos.second) % 2)){
                            // two bishops on same colour diagonal
                            twoBishopDeadPos = true;
                        }
                        numBishopOrKnight++;
                    }
                    else if(toupper(board[i][j]->getName()) != 'K'){
                        // If we have some other piece then we ignore
                        numBishopOrKnight=3;
                    }
                }
            }
        }
        if(numBishopOrKnight == 1 || (numBishopOrKnight == 2 && twoBishopDeadPos) || numBishopOrKnight == 0){
            std::cout << "Dead Position. " << std::endl;
            winner = "tie";
            notifyObservers();
            initChessboard();
            return true;
        }
        if(fiftyMoveDrawCount >= 50){
            std::cout << "Fifty Move Draw. " << std::endl;
            winner = "tie";
            notifyObservers();
            initChessboard();
            return true;
        }
    }

    // Increment 50 move counter if no pawn moved and no capture made
    if(prevBoard[row2][col2] == nullptr && toupper(prevBoard[row1][col1]->getName()) != 'P'){
        fiftyMoveDrawCount++;
    }
    else{
        fiftyMoveDrawCount=0;
    }

    // notifyObservers
    notifyObservers();
    return false;
}

std::string Chessboard::getWinner(){
    return winner;
}

bool Chessboard::validChessboard(){
    // validate that the board contains exactly one white king and exactly one black
    // king; that no pawns are on the first or last row of the board; and that neither 
    // king is in check. The user cannot leave setup mode until these conditions are satisfied.
    bool blackKing, whiteKing;
    blackKing = false;
    whiteKing = false;

    for(size_t i = 0; i < Chessboard::BOARD_SIZE; i++){
        for(size_t j = 0; j < Chessboard::BOARD_SIZE; j++){
            if(board[i][j] != nullptr){
                char piece = board[i][j]->getName();

                if(piece == 'K'){
                    if(whiteKing){
                        return false;
                    }
                    whiteKing = true;
                    if (isKingInCheck('W')) {
                        return false;
                    }
                }
                if(piece == 'k'){
                    if(blackKing){
                        return false;
                    }
                    blackKing = true;
                    if (isKingInCheck('B')) {
                        return false;
                    }
                }
                if(piece == 'P' || piece == 'p'){
                    if(i == 0 || i == 7){
                        return false;
                    }
                }
            }
        }
    }
    
    return blackKing && whiteKing;
}

std::pair<const Piece*, const Piece*> Chessboard::getState(size_t row, size_t col) const{
    return std::pair<const Piece*, const Piece*>(board[row][col].get(), prevBoard[row][col].get());
}

void Chessboard::clearChessboard() {
    for (size_t i = 0; i < BOARD_SIZE; i++) {
        for (size_t j = 0; j < BOARD_SIZE; j++) {
            if(board[i][j]) prevBoard[i][j] = std::move(board[i][j]);
        }
    }
    fiftyMoveDrawCount = 0;
}

void Chessboard::initChessboard() {
    clearChessboard();

    // White pieces
    board[0][0] = std::make_unique<Rook>('W');
    board[0][1] = std::make_unique<Knight>('W');
    board[0][2] = std::make_unique<Bishop>('W');
    board[0][3] = std::make_unique<Queen>('W');
    board[0][4] = std::make_unique<King>('W');
    board[0][5] = std::make_unique<Bishop>('W');
    board[0][6] = std::make_unique<Knight>('W');
    board[0][7] = std::make_unique<Rook>('W');
    for (size_t i = 0; i < Chessboard::BOARD_SIZE; i++) {
        board[1][i] = std::make_unique<Pawn>('W');;
    }

    // Black pieces
    board[7][0] = std::make_unique<Rook>('B');
    board[7][1] = std::make_unique<Knight>('B');
    board[7][2] = std::make_unique<Bishop>('B');
    board[7][3] = std::make_unique<Queen>('B');
    board[7][4] = std::make_unique<King>('B');
    board[7][5] = std::make_unique<Bishop>('B');
    board[7][6] = std::make_unique<Knight>('B');
    board[7][7] = std::make_unique<Rook>('B');
    for (size_t i = 0; i < Chessboard::BOARD_SIZE; i++) {
        board[6][i] = std::make_unique<Pawn>('B');;
    }
}

bool Chessboard::validPiece(std::string piece) {
    return (isWhitePiece(piece) || isBlackPiece(piece));
}

bool Chessboard::validGridPosition(std::string position) {
    return (gridLetters.find(position[0]) != gridLetters.end() && gridNumbers.find(position[1]) != gridNumbers.end());
}

bool Chessboard::isBlackPiece(std::string piece){
    return (validBlackPieceInputs.find(piece[0]) != validBlackPieceInputs.end());
}

bool Chessboard::isWhitePiece(std::string piece){
    return (validWhitePieceInputs.find(piece[0]) != validWhitePieceInputs.end());
}

bool Chessboard::validPromotionPiece(std::string piece, char color) {
    if(color == 'W') {
        return isWhitePromotionPiece(piece);
    }
    else if (color == 'B') {
        return isBlackPromotionPiece(piece);
    }
    return false;
}

bool Chessboard::isBlackPromotionPiece(std::string piece){
    return (promotedBlackPieces.find(piece[0]) != promotedBlackPieces.end());
}

bool Chessboard::isWhitePromotionPiece(std::string piece){
    return (promotedWhitePieces.find(piece[0]) != promotedWhitePieces.end());
}

std::unique_ptr<Piece> Chessboard::createPiece(std::string piece, char color) {
    switch (piece[0]) {
        case 'K': case 'k': return std::make_unique<King>(color);
        case 'Q': case 'q': return std::make_unique<Queen>(color);
        case 'R': case 'r': return std::make_unique<Rook>(color);
        case 'B': case 'b': return std::make_unique<Bishop>(color);
        case 'N': case 'n': return std::make_unique<Knight>(color);
        case 'P': case 'p': return std::make_unique<Pawn>(color);
        default: throw InternalErrorException{"Internal Error: Invalid piece type"};
    }
}

void Chessboard::makeMove(int startRow, int startCol, int endRow, int endCol, bool enPassant) {
    if(enPassant) {
        std::unique_ptr<Piece> tmp;
        tmp = std::move(board[startRow][startCol]);
    }
    else {
        std::unique_ptr<Piece> tmp;
        tmp = std::move(board[endRow][endCol]);
    }
    board[endRow][endCol] = std::move(board[startRow][startCol]);
}


bool Chessboard::isKingInCheck(char kingColour) {
    int kingX = -1, kingY = -1;

    for (size_t i = 0; i < Chessboard::BOARD_SIZE; i++) {
        for (size_t j = 0; j < Chessboard::BOARD_SIZE; j++) {
            Piece* piece = board[i][j].get();
            if (piece != nullptr && piece->getName() == (kingColour == 'W' ? 'K' : 'k')) {
                kingX = j;
                kingY = i;
                break;
            }
        }
        if (kingX != -1) break; // Exit the loop once the king is found
    }

    if(kingX == -1 || kingY == -1) {
        throw InternalErrorException{"Internal Error: King not found"};
    }

    return isPieceBeingAttacked(kingX, kingY, kingColour);
}

bool Chessboard::isPieceBeingAttacked(int x, int y, char colour) {
    // Check if any opponent piece is attacking the position (x, y)
    for (int i = 0; i < static_cast<int>(Chessboard::BOARD_SIZE); i++) {
        for (int j = 0; j < static_cast<int>(Chessboard::BOARD_SIZE); j++) {
            Piece* piece = board[i][j].get();

            if (piece != nullptr) {
                if(piece->getColour() != colour) {
                    int relativeX = x - j;
                    int relativeY = y - i;

                    if(toupper(piece->getColour()) == 'B') {
                        relativeY *= -1;
                    }

                    if (piece->isValidMove(relativeX, relativeY)) {
                        char pieceName = piece->getName();

                        switch(pieceName) {
                            case 'Q': case 'q': 
                                if(j == x || i == y){
                                    // Vertical queen movement
                                    if (isStraightClear(j, i, x, y)) {
                                        return true;
                                    }
                                }
                                else{
                                    // Diagonal queen movement
                                    if (isDiagonalClear(j, i, x, y)) {
                                        return true;
                                    }
                                }
                                break;
                            case 'R': case 'r': 
                                if(isStraightClear(j, i, x, y)) {
                                    return true;
                                }
                                break;
                            case 'B': case 'b': 
                                if(isDiagonalClear(j, i, x, y)) {
                                    return true;
                                }
                                break;
                            case 'N': case 'n': 
                                if ((abs(relativeX) == 2 && abs(relativeY) == 1) || (abs(relativeX) == 1 && abs(relativeY) == 2)) {
                                    return true;
                                }
                                break;
                            case 'P': case 'p': 
                                if (abs(relativeX) == 1 && relativeY == 1) {
                                    return true;
                                }
                                break;
                            case 'K': case 'k':
                                if(abs(relativeX) == 1 || abs(relativeY) == 1){
                                    return true;
                                }
                            default: 
                                // throw InternalErrorException{"Internal Error: isPieceBeingAttacked Broken"};
                                break;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool Chessboard::isStraightClear(int startX, int startY, int TargetX, int TargetY) {
    if (startX != TargetX && startY != TargetY) {
        return true;
    }

    int squaresBetween = 0, factor = 0;

    // Calculate direction and distance
    if(startX == TargetX) {
        squaresBetween = abs(TargetY - startY) - 1;
        factor = (TargetY - startY) / abs(TargetY - startY);
    }
    else {
        squaresBetween = abs(TargetX - startX) - 1;
        factor = (TargetX - startX) / abs(TargetX - startX);
    }

    // Check each square along the path
    if (startX == TargetX) {
        // Vertical movement
        for (int i = 1; i <= squaresBetween; i++) {
            if (board[startY + i*factor][startX] != nullptr) {
                return false;
            }
        }
    } else {
        // Horizontal movement
        for (int i = 1; i <= squaresBetween; i++) {
            if (board[startY][startX + i*factor] != nullptr) {
                return false;
            }
        }
    }
    return true;
}


bool Chessboard::isDiagonalClear(int startX, int startY, int TargetX, int TargetY) {
    // not diagonal if this condition is met
    if(abs(startX - TargetX) != abs(startY - TargetY)) {
        return false;
    }

    // calculate number of squares between start and end
    int squaresBetween = abs(TargetX - startX) - 1;

    // will be 1 or -1 depending on direction
    int xFactor = (TargetX - startX) / abs(TargetX - startX);
    int yFactor = (TargetY - startY) / abs(TargetY - startY);

    // check each square along the path
    for (int i = 1; i<=squaresBetween; i++) {
        if (board[startY + i*yFactor][startX + i*xFactor] != nullptr) {
            return false;
        }
    }
    return true;
}

Chessboard::~Chessboard(){}
