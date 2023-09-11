
#include "types.h"


class PrettyPrinter: public ExprVisitor {

private: 
    Value* parenthesize(std::string name, std::vector<Expr*> exprs, ExprVisitor& visitor)  { 

        std::cout << "(" << name;
        
        for (Expr* expr : exprs) {
            std::cout  << " ";
            // std::cout << "something";
            expr->accept(visitor);
            std::cout  << " ";
        }
        
        std::cout  << ")";

        return new Value();

    }
public:
    Value* visitBinary(Binary& expr) {
      //cout some things
    std::vector<Expr*> exprs = {&expr.left, &expr.right}; 
      return parenthesize(expr.oper.lexeme, exprs, *this);
    }

    Value* visitGrouping(Grouping& expr) {
        std::cout << "Visiting Grouping" ;
        std::vector<Expr*> exprs = {&expr.expression}; 
        return parenthesize("group", exprs, *this);
    }

    Value* visitLiteral(Literal& expr) {
        std::cout << expr.value ;
        return new Value();
    }

    Value* visitUnary(Unary& expr) {
        std::vector<Expr*> exprs = {&expr.right};
        return parenthesize(expr.oper.lexeme, exprs, *this);
    }
};