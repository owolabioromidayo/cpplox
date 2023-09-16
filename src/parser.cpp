// #include "types.hpp"
#include <stdexcept>
#include <vector>

#include "parser.hpp"

std::vector<Statement*> Parser::parse(){
    std::vector<Statement*> statements;
    while(!isAtEnd()){
        statements.push_back(declaration());
    }
    return statements;
}
Statement* Parser::declaration(){
    try {
        if (match(TokenType::VAR)) return varDeclaration();
        if (match(TokenType::FUN)) return funDeclaration("function ");
        return statement();

    }catch(ParseError error){
        synchronize();
        return new ExprStmt(new Literal( "nil", TokenType::NIL));
    }
}


Statement* Parser::funDeclaration(std::string kind){
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");

    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 255) {
                error(peek(), "Can't have more than 255 parameters.");
            }
            parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<Statement*> body = dynamic_cast<BlockStmt*>(block())->statements;
    return new FunctionStmt(name, parameters, body);
}

Statement* Parser::varDeclaration(){
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    Expr* initializer = nullptr;

    if (match(TokenType::EQUAL)) {
        initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return new VarStmt(name, initializer);
}

Statement* Parser::statement(){
    if (match(TokenType::PRINT)) return printStatement();
    if (match(TokenType::LEFT_BRACE)) return block();
    if (match(TokenType::IF)) return ifStatement();
    if (match(TokenType::WHILE)) return whileStatement();
    if (match(TokenType::FOR)) return forStatement();
    if (match(TokenType::RETURN)) return returnStatement();
    return expressionStatement();
}


Statement* Parser::returnStatement() {
    Token keyword = previous();
    Expr* value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return new ReturnStmt(keyword, value);
}

Statement* Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    Expr* condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
    Statement* thenBranch = statement();
    Statement* elseBranch = nullptr;
    if (match(TokenType::ELSE)) 
        elseBranch = statement();
    
    return new IfStmt(condition, thenBranch, elseBranch);
}

Statement* Parser::forStatement() {
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

Statement* Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    Expr* condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    Statement* body = statement();
    return new WhileStmt(condition, body);
}

Statement* Parser::printStatement(){
    Expr* value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value. ");
    return new PrintStmt(value);
}

Statement* Parser::block(){
    std::vector<Statement*> statements;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return new BlockStmt(statements);
    
}

Statement* Parser::expressionStatement(){
    Expr* value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value. ");
    return new ExprStmt(value);
}

Expr* Parser::expression() {
    return assignment();
}

Expr* Parser::assignment(){
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

Expr* Parser::or_(){
    Expr* expr = and_();
    while (match(TokenType::OR)) {
        Token oper = previous();
        Expr* right = and_();
        expr = new Logical(expr, oper, right);
    }
    return expr; 
}

Expr* Parser::and_(){ 
    Expr* expr = equality();

    while (match(TokenType::AND)) {
        Token oper = previous();
        Expr* right = equality();
        expr = new Logical(expr, oper, right);
    }
    return expr;
}

Expr* Parser::equality(){
    Expr* expr = comparison();

    std::vector<TokenType> exprs = {TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL};

    while (match(exprs) ) {
        Token oper = previous();
        Expr* right = comparison();
        expr = new Binary(expr, oper, right);   
    }

    return expr;
}

Expr* Parser::comparison(){
    Expr* expr = term();

    std::vector<TokenType> exprs = {TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL};
    while (match(exprs)){
        Token oper = previous();
        Expr* right = term();
        expr = new Binary(expr, oper, right);   
    }

    return expr;
}

Expr* Parser::term(){
    Expr* expr = factor();

    std::vector<TokenType> exprs = {TokenType::MINUS, TokenType::PLUS};
    while (match(exprs)){
        Token oper = previous();
        Expr* right = factor();
        expr = new Binary(expr, oper, right);   
    }

    return expr;
}

Expr* Parser::factor(){
    Expr* expr = unary();

    std::vector<TokenType> exprs = {TokenType::SLASH, TokenType::STAR};
    while (match(exprs)){
        Token oper = previous();
        Expr* right = unary();
        expr = new Binary(expr, oper, right);   
    }

    return expr;
}

Expr* Parser::unary(){
    std::vector<TokenType> exprs = {TokenType::BANG, TokenType::MINUS};
    if (match(exprs)){
        Token oper = previous();
        Expr* right = unary();
        return new Unary(oper, right);   
    }

    return call();
}

Expr* Parser::call(){
    Expr* expr = primary();

    while(true){
        if (match(TokenType::LEFT_PAREN)){
            expr = finishCall(expr);
        } else {
            break;
        }
    }

    return expr;
}

Expr* Parser::primary(){
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

Expr* Parser::finishCall(Expr* callee){
    std::vector<Expr*> arguments;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if(arguments.size() >= 255)
                error(peek(), "Cant have more than 255 arguments");
            arguments.push_back(expression());
        } while (match(TokenType::COMMA));
    }
    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    return new Call(callee, dynamic_cast<Variable*>(callee)->name, arguments);
}

bool Parser::isAtEnd() { 
    return peek().type == TokenType::EOF_;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}


bool Parser::match(TokenType type){
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(std::vector<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}


Token Parser::consume(TokenType type, std::string message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}


ParseError Parser::error(Token token, std::string message) {
    report(token.line, "", message);
    return ParseError("");
}


void  Parser::synchronize() {
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

