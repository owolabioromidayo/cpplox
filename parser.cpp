// #include "types.h"
#include <stdexcept>

class Binary; 
class Grouping; 
class Literal; 
class Unary; 

class ExprVisitor {
public:
    virtual void visitBinary(Binary& expr) = 0;
    virtual void visitGrouping(Grouping& expr) = 0;
    virtual void visitLiteral(Literal& expr) = 0;
    virtual void visitUnary(Unary& expr) = 0;
    virtual ~ExprVisitor() {}
};


class Expr {
public:
    ~Expr() {} 
    virtual void accept(ExprVisitor& visitor) = 0;
};

class Binary : public Expr {
public:
    Binary(Expr& left, Token& oper, Expr& right)
        : left(left), oper(oper), right(right) {}

    Binary(Expr*& left, Token& oper, Expr*& right) 
    : left(*left), oper(oper), right(*right) {}   


    Expr& left;
    Token oper; 
    Expr& right;

    void accept(ExprVisitor& visitor) {
        visitor.visitBinary(*this);
    }
};


class Grouping : public Expr {
public:
    Grouping(Expr& expression)
        : expression(expression) {}

    Grouping(Expr*& expression)
        : expression(*expression) {}

    Expr& expression;

    void accept(ExprVisitor& visitor) {
        visitor.visitGrouping(*this);
    }
};

class Literal : public Expr {
public:
    Literal(std::string value, TokenType type)
        : value(value), type(type) {}

    TokenType type;
    std::string value;

    void accept(ExprVisitor& visitor) {
        visitor.visitLiteral(*this);
    }
};

class Unary : public Expr {
public:
    Unary(Token& oper, Expr& right)
        : oper(oper), right(right) {}

    Unary(Token& oper, Expr*& right)
        : oper(oper), right(*right) {} 
    
   
    Token oper;
    Expr& right;

    void accept(ExprVisitor& visitor) {
        visitor.visitUnary(*this);
    }
};




class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message) : std::runtime_error(message) {}
};


class Parser {
public:
    std::vector<Token> tokens;
    int current  = 0;

    Parser (std::vector<Token> tokens): tokens(tokens) {}


    Expr* parse(){
        try{
            return expression();
        } catch(ParseError error){
            return new Literal("", TokenType::NIL);
        }
    }
private:
    Expr* expression() {
        return equality();
    }


    Expr* equality(){
        Expr* expr = comparison();

        std::vector<TokenType> exprs = {TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL};

        while (match(exprs) ) {
            Token oper = previous();
            Expr* right = comparison();
            expr = new Binary(expr, oper, right);   
        }

        return expr;
    }

    Expr* comparison(){
        Expr* expr = term();

        std::vector<TokenType> exprs = {TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL};
        while (match(exprs)){
            Token oper = previous();
            Expr* right = term();
            expr = new Binary(expr, oper, right);   
        }

        return expr;
    }

    Expr* term(){
        Expr* expr = factor();

        std::vector<TokenType> exprs = {TokenType::MINUS, TokenType::PLUS};
        while (match(exprs)){
            Token oper = previous();
            Expr* right = factor();
            expr = new Binary(expr, oper, right);   
        }

        return expr;
    }

    Expr* factor(){
        Expr* expr = unary();

        std::vector<TokenType> exprs = {TokenType::SLASH, TokenType::STAR};
        while (match(exprs)){
            Token oper = previous();
            Expr* right = unary();
            expr = new Binary(expr, oper, right);   
        }

        return expr;
    }

    Expr* unary(){
        std::vector<TokenType> exprs = {TokenType::BANG, TokenType::BANG};
        if (match(exprs)){
            Token oper = previous();
            Expr* right = unary();
            return new Unary(oper, right);   
        }

        return primary();
    }

    Expr* primary(){
        if (match(TokenType::FALSE)) return new Literal("false", TokenType::FALSE);
        if (match(TokenType::TRUE)) return new Literal("true", TokenType::TRUE);
        if (match(TokenType::NIL)) return new Literal("nil", TokenType::NIL);


        std::vector<TokenType> exprs = {TokenType::NUMBER, TokenType::STRING};
        if (match(exprs)){
            return new Literal(previous().literal, previous().type);
        }

        if (match(TokenType::LEFT_PAREN)) {
            Expr* expr = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression ");
            return new Grouping(expr);
        }

        throw error(peek(), "Expect expression.");
    }

    
    bool isAtEnd() { 
        return peek().type == TokenType::EOF_;
    }

    Token peek() {
        return tokens[current];
    }

    Token previous() {
        return tokens[current - 1];
    }

    bool check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }


    bool match(TokenType type){
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

    bool match(std::vector<TokenType> types) {
        for (TokenType type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }


    Token consume(TokenType type, std::string message) {
        if (check(type)) return advance();
        throw error(peek(), message);
    }

    void report(int line, const std::string& where, const std::string& message) {
        std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
        hadError = true;
    }

    ParseError* error(Token token, std::string message) {
        report(token.line, "", message);
        return new ParseError("");
    }


    void synchronize() {
        advance();

        while(!isAtEnd()) {
            if (previous().type == TokenType::SEMICOLON) return;

            switch(peek().type) {
                case TokenType::CLASS:
                case TokenType::FUN:
                case TokenType::VAR:
                case TokenType::FOR:
                case TokenType::IF:
                case TokenType::WHILE:
                case TokenType::PRINT:
                case TokenType::RETURN:
                    return;
            }

            advance();
        }
    }
};
