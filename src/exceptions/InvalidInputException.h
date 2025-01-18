#ifndef INVALIDINPUTEXCEPTION_H
#define INVALIDINPUTEXCEPTION_H

#include <exception>
#include <string>

class InvalidInputException : public std::exception {
private:
    std::string message;
public:
    InvalidInputException(const std::string& msg) : message(msg) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif
