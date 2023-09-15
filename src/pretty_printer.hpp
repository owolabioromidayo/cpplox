#ifndef PRETTY_PRINTER__
#define PRETTY_PRINTER__
#include "types.hpp"


class PrettyPrinter: public ExprVisitor {

private: 
    Value* parenthesize(std::string name, std::vector<Expr*> exprs, ExprVisitor& visitor)  { 

        std::ostringstream oss;
        oss << "(" << name;
        
        for (Expr* expr : exprs) {
            oss  << " ";
            expr->accept(visitor);
            oss  << " ";
        }
        
        oss  << ")";

        return new Value(oss.str());

    }
public:
    Value* visitBinary(Binary& expr) {
    std::vector<Expr*> exprs = {&expr.left, &expr.right}; 
      return parenthesize(expr.oper.lexeme, exprs, *this);
    }

    Value* visitGrouping(Grouping& expr) {
        std::vector<Expr*> exprs = {&expr.expression}; 
        return parenthesize("group", exprs, *this);
    }

    Value* visitLiteral(Literal& expr) {
        std::cout << expr.value ;
        return new Value(expr.value);
    }

    Value* visitUnary(Unary& expr) {
        std::vector<Expr*> exprs = {&expr.right};
        return parenthesize(expr.oper.lexeme, exprs, *this);
    }

    
    Value* visitVariable(Variable& expr) {
        return new Value(expr.name.lexeme);
    };

     Value* visitAssign(Assign& expr) {
        return new Value(expr.name.lexeme + " = " + expr.value->accept(*this)->view() );
    };

    virtual Value* visitLogicalExpr(Logical& expr) {
      return new Value( expr.left->accept(*this)->view() + " " + expr.oper.lexeme + " " + expr.right->accept(*this)->view() );  
    };
};


#endif // PRETTY_PRINTER__