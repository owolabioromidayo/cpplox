// #include "types.hpp"
#include <stdexcept>
#include <vector>
#include "types.hpp"
#include "error.hpp"

#include "pretty_printer.hpp"




class Parser {
public:
    std::vector<Token> tokens;
    int current  = 0;

    Parser (std::vector<Token> tokens): tokens(tokens) {}


    // Expr* parse(){
    //     try{
    //         return expression();
    //     } catch(ParseError error){
    //         return new Literal("", TokenType::NIL);
    //     }
    // }

    std::vector<Statement*> parse(){
        std::vector<Statement*> statements;
        while(!isAtEnd()){
            statements.push_back(declaration());
        }
        return statements;
    }
private:
    Statement* declaration(){
        try {
            if (match(TokenType::VAR)) return varDeclaration();
            return statement();

        }catch(ParseError error){
            synchronize();
            return new ExprStmt(new Literal( "nil", TokenType::NIL));
        }
    }

    Statement* varDeclaration(){
        Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

        Expr* initializer = nullptr;

        if (match(TokenType::EQUAL)) {
            initializer = expression();
        }
        consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
        return new VarStmt(name, initializer);
    }
    Statement* statement(){
        if (match(TokenType::PRINT)) return printStatement();
        if (match(TokenType::LEFT_BRACE)) return block();
        if (match(TokenType::IF)) return ifStatement();
        if (match(TokenType::WHILE)) return whileStatement();
        if (match(TokenType::FOR)) return forStatement();
        return expressionStatement();
    }

    Statement* ifStatement() {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
        Expr* condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
        Statement* thenBranch = statement();
        Statement* elseBranch = nullptr;
        if (match(TokenType::ELSE)) 
            elseBranch = statement();
        
        return new IfStmt(condition, thenBranch, elseBranch);
    }

    Statement* forStatement() {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");

        Statement* initializer;
        if (match(TokenType::SEMICOLON)) {
            initializer = nullptr;
        } else if (match(TokenType::VAR)) {
            initializer = varDeclaration();
        } else {
            initializer = expressionStatement();
        }

        Expr* condition = nullptr;

        if (!check(TokenType::SEMICOLON))
            condition = expression();

        consume(TokenType::SEMICOLON, "Expect ';' after value. ");

        Expr* increment = nullptr;
        if (!check(TokenType::RIGHT_PAREN)) {
            increment = expression();

        }
        consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

        Statement* body = statement();

        if (increment != nullptr) {
            Statement* expr = new ExprStmt(increment);
            std::vector< Statement*> statements = {body, expr};
            body = new BlockStmt(statements);
        }

        body = new WhileStmt(condition, body);
        if (initializer != nullptr){
            std::vector<Statement* > statements = {initializer, body}; 
            body = new BlockStmt(statements); 
        }
        
        return body;
    }

    Statement* whileStatement() {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
        Expr* condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
        Statement* body = statement();
        return new WhileStmt(condition, body);
    }

    Statement* printStatement(){
        Expr* value = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after value. ");
        return new PrintStmt(value);
    }

    Statement* block(){
        std::vector<Statement*> statements;

        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            statements.push_back(declaration());
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
        return new BlockStmt(statements);
       
    }

    Statement* expressionStatement(){
        Expr* value = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after value. ");
        return new ExprStmt(value);
    }

    Expr* expression() {
        return assignment();
    }

    Expr* assignment(){
        Expr* expr = or_();

        if(match(TokenType::EQUAL)){
            Token equals = previous();
            Expr* value = assignment();

            if (dynamic_cast<const Variable*>(expr) != nullptr){
                Token name = dynamic_cast<Variable*>(expr)->name; 
                return new Assign(name, value);
            }

        error(equals, "Invalid assignment target"); 
        }
        return expr; 
    }

    Expr* or_(){
        Expr* expr = and_();
        while (match(TokenType::OR)) {
            Token oper = previous();
            Expr* right = and_();
            expr = new Logical(expr, oper, right);
        }
        return expr; 
    }

    Expr* and_(){ 
        Expr* expr = equality();

        while (match(TokenType::AND)) {
            Token oper = previous();
            Expr* right = equality();
            expr = new Logical(expr, oper, right);
        }
        return expr;
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
        std::vector<TokenType> exprs = {TokenType::BANG, TokenType::MINUS};
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
        if (match(TokenType::IDENTIFIER)) return new Variable(previous());

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
