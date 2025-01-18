#include "Subject.h"

void Subject::attach(std::unique_ptr<Observer> o){
    observers.push_back(std::move(o));
}

void Subject::detach(std::unique_ptr<Observer> o){
    for(auto it = observers.begin(); it != observers.end(); it++){
        if(*it == o){
            observers.erase(it);
            break;
        }
    }
}

void Subject::notifyObservers(){
    for(std::unique_ptr<Observer> &observer : observers){
        observer->notify();
    }
}

Subject::~Subject(){}
