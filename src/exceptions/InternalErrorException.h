#ifndef INTERNALERROREXCEPTION_H
#define INTERNALERROREXCEPTION_H

#include <exception>
#include <string>

class InternalErrorException : public std::exception {
private:
    std::string message;
public:
    InternalErrorException(const std::string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif 
