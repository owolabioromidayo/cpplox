#ifndef TYPE_HEADER_H
#define TYPE_HEADER_H

#include <iostream>
#include <string>
#include <sstream> // For std::ostringstream


#include <vector>
#include <fstream>
#include <cctype>
#include <unordered_map>

enum class ValueType{ 
    STRING, NUMBER, NIL, BOOLEAN
};

const std::unordered_map<ValueType, std::string> ValueTypeToStringMap = {
    {ValueType::STRING, "string"},
    {ValueType::NUMBER, "number"},
    {ValueType::NIL, "nil"},
    {ValueType::BOOLEAN, "boolean"},
};

std::string valueTypeToString(ValueType value) {
    auto it = ValueTypeToStringMap.find(value);
    if (it != ValueTypeToStringMap.end()) {
        return it->second;
    } else {
        return "Unknown"; // Handle unknown enum values
    }
}

struct Value{
    ValueType type;
    union {
        double number;
        std::string str; 
        bool bool_;
    };

    Value(int value) : type(ValueType::NUMBER), number(value) {}
    Value(double value) : type(ValueType::NUMBER), number(value) {}
    Value(bool value) : type(ValueType::BOOLEAN), bool_(value) {}
    Value() : type(ValueType::NIL) {}
    Value(const std::string& value) : type(ValueType::STRING), str(value) {}

    std::string view(){
       std::ostringstream oss;
       oss << "{ " << valueTypeToString(type) << " " ;
       switch (type){
            case ValueType::NUMBER:
                oss << number;
                break;
            case ValueType::STRING:
                oss << str;
                break;
            case ValueType::BOOLEAN:
                oss << bool_;
                break;
       }
       oss << " }";

       return oss.str();
    }

    ~Value() {
        if (type == ValueType::STRING) {
            str.~basic_string();
        }
    }
};

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    // Literals.
    IDENTIFIER, STRING, NUMBER,
    // Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,
    EOF_ 
};

std::unordered_map<std::string, TokenType> keywords = {
    {"and", TokenType::AND},
    {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},
    {"false", TokenType::FALSE},
    {"for", TokenType::FOR},
    {"fun", TokenType::FUN},
    {"if", TokenType::IF},
    {"nil", TokenType::NIL},
    {"or", TokenType::OR},
    {"print", TokenType::PRINT},
    {"return", TokenType::RETURN},
    {"super", TokenType::SUPER},
    {"this", TokenType::THIS},
    {"true", TokenType::TRUE},
    {"var", TokenType::VAR},
    {"while", TokenType::WHILE}
};

// we have NULL, string, double(nuumber) literals. we can always specify the fromstr conversion later
class Token {
public:
    TokenType type;
    std::string lexeme;
    std::string literal;
    int line;

    //literal handling seems very broken, thanks cpp and no reflection

    Token(TokenType type, const std::string& lexeme, const std::string& literal, int line) :
        type(type), lexeme(lexeme), literal(literal), line(line) {}

    std::string toString() {
        return typeid(type).name() + std::string(" ") + lexeme + " " + literal;
    }
};


class Binary; 
class Grouping; 
class Literal; 
class Unary; 
class ExprStmt;
class PrintStmt;

class ExprVisitor {
public:
    virtual Value* visitBinary(Binary& expr) = 0;
    virtual Value* visitGrouping(Grouping& expr) = 0;
    virtual Value* visitLiteral(Literal& expr) = 0;
    virtual Value* visitUnary(Unary& expr) = 0;
    virtual ~ExprVisitor() {}
};

class StmtVisitor{
public:
    virtual void visitPrintStmt(PrintStmt& stmt) = 0;     
    virtual void visitExprStmt(ExprStmt& stmt) = 0;     
};

class Expr {
public:
    ~Expr() {} 
    virtual Value* accept(ExprVisitor& visitor) = 0;
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

    Value* accept(ExprVisitor& visitor) {
        return visitor.visitBinary(*this);
    }
};


class Grouping : public Expr {
public:
    Grouping(Expr& expression)
        : expression(expression) {}

    Grouping(Expr*& expression)
        : expression(*expression) {}

    Expr& expression;

    Value* accept(ExprVisitor& visitor) {
        return visitor.visitGrouping(*this);
    }
};

class Literal : public Expr {
public:
    Literal(std::string value, TokenType type)
        : value(value), type(type) {}

    TokenType type;
    std::string value;

    Value* accept(ExprVisitor& visitor) {
        return visitor.visitLiteral(*this);
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

    Value* accept(ExprVisitor& visitor) {
        return visitor.visitUnary(*this);
    }
};

class Statement {
public:
    ~Statement() {} 
    virtual void accept(StmtVisitor& visitor) = 0;
};


class ExprStmt : public Statement {
public:
    ExprStmt(Expr* expression): expression(expression) {};
    Expr* expression;

    void accept(StmtVisitor& visitor) {
        return visitor.visitExprStmt(*this);
    }
};


class PrintStmt : public Statement {
public:
    PrintStmt(Expr* expression): expression(expression) {};
    Expr* expression;

    void accept(StmtVisitor& visitor) {
        return visitor.visitPrintStmt(*this);
    }
};
#endif //TYPE_HEADER_H