#ifndef SCANNER_H_
#define SCANNER_H_

#include "types.hpp"

class Scanner{
public:
    Scanner(const std::string& source) : source(source) {}
    std::vector<Token> scanTokens();
    

private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0; 
    int current = 0;
    int line = 1;

    bool isAtEnd();
    void scanToken();


    void identifier();
    void number();
    void string();

    char peek();
    char peekNext();
    bool match(char expected);
    char advance();

    
    void addToken(TokenType type); // w no literal
    void addToken(TokenType type, std::string literal); //w literal
};

#endif //SCANNER_H_ 