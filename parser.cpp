// #include "types.h"

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
    
   
    Token oper;
    Expr& right;

    void accept(ExprVisitor& visitor) {
        visitor.visitUnary(*this);
    }
};


