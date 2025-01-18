#ifndef SUBJECT_H
#define SUBJECT_H

#include <vector>
#include <iostream>
#include <memory>
#include "Observer.h"

// Forward Declarations
class Piece;

class Subject{
    std::vector<std::unique_ptr<Observer>> observers;
    public:
        void attach(std::unique_ptr<Observer> o);
        void detach(std::unique_ptr<Observer> o);
        void notifyObservers();
        virtual std::pair<const Piece*, const Piece*> getState(size_t row, size_t col) const = 0;
        virtual ~Subject();
};

#endif 
