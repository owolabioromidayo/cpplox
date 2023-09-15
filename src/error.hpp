#ifndef ERROR_H_
#define ERROR_H_

#include "types.hpp"

class RuntimeError: public std::runtime_error {
public:
    RuntimeError(Token token, const std::string& message) : std::runtime_error(message), token(token), message(message) {};
    Token token;
    std::string message;

    ~RuntimeError(){}
};

class Return: public std::runtime_error {
public:
    Return(Value* value) : value(value) , std::runtime_error("") {};
    Value*  value;
};

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message) : std::runtime_error(message) {}
    ~ParseError(){}
};


bool hadError = false; 

void report(int line, const std::string& where, const std::string& message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
    hadError = true;
}

void error(int line, const std::string& message) {
    report(line, "", message);
}

#endif // ERROR_H_