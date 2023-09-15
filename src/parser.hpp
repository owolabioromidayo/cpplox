#ifndef PARSER_H_
#define PARSER_H_

#include "types.hpp"
#include "error.hpp"
#include "pretty_printer.hpp"


class Parser {
public:
    std::vector<Token> tokens;
    int current  = 0;

    Parser (std::vector<Token> tokens): tokens(tokens) {};
    std::vector<Statement*> parse();

private:

    Statement* declaration();
    Statement* funDeclaration(std::string kind);
    Statement* varDeclaration();
    Statement* statement();
    Statement* returnStatement();
    Statement* ifStatement();
    Statement* forStatement();
    Statement* whileStatement();
    Statement* printStatement();
    Statement* block();
    Statement* expressionStatement();

    Expr* expression();
    Expr* assignment();
    Expr* or_();
    Expr* and_();
    Expr* equality();
    Expr* comparison();
    Expr* term();
    Expr* factor();
    Expr* unary();
    Expr* call();
    Expr* primary();
    Expr* finishCall(Expr* callee);

    bool isAtEnd() ;
    Token peek() ;
    Token previous() ;
    bool check(TokenType type) ;
    Token advance() ;
    bool match(TokenType type);
    bool match(std::vector<TokenType> types) ;
    Token consume(TokenType type, std::string message) ;
    ParseError error(Token token, std::string message) ;

    void synchronize() ;

};


#endif //PARSER_H_