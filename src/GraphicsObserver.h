#ifndef GRAPHICSOBSERVER_H
#define GRAPHICSOBSERVER_H

#include "Observer.h"
#include "Chessboard.h"
#include "Window.h"
#include <unordered_map>

// Images MUST be 50*50 in size (15bit RGBA)
using array_type = const unsigned int[50*50];

struct Theme {
    int lightSquareColour;
    int darkSquareColour;
};

class GraphicsObserver: public Observer {
    const Chessboard *subject;
    std::unique_ptr<Xwindow> window;
    array_type& getPieceImgArray(char piece);
    Theme currentTheme;

    public:
        inline static const int GRID_SIZE = 50;
        GraphicsObserver(const Chessboard *board, const Theme& theme);
        void notify() override;
        ~GraphicsObserver();

        static const std::unordered_map<std::string, Theme> themes;
};

#endif
