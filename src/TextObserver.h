#ifndef TEXTOBSERVER_H
#define TEXTOBSERVER_H

#include "Observer.h"

class Chessboard;

class TextObserver : public Observer{
    const Chessboard* subject;
    public:
        TextObserver(const Chessboard*);
        void notify() override;
};

#endif
