// #include "parser.cpp"
#include <sstream> // For std::ostringstream




class PrettyPrinter: public ExprVisitor {

private: 
    void parenthesize(std::string name, std::vector<Expr*> exprs, ExprVisitor& visitor)  { 

        std::cout << "(" << name;
        
        for (Expr* expr : exprs) {
            std::cout  << " ";
            // std::cout << "something";
            expr->accept(visitor);
            std::cout  << " ";
        }
        
        std::cout  << ")";

    }
public:
    void visitBinary(Binary& expr) {
      //cout some things
    std::vector<Expr*> exprs = {&expr.left, &expr.right}; 
      return parenthesize(expr.oper.lexeme, exprs, *this);
    }

    void visitGrouping(Grouping& expr) {
        std::cout << "Visiting Grouping" ;
        std::vector<Expr*> exprs = {&expr.expression}; 
        return parenthesize("group", exprs, *this);
    }

    void visitLiteral(Literal& expr) {
        std::cout << expr.value ;
    }

    void visitUnary(Unary& expr) {
        std::vector<Expr*> exprs = {&expr.right};
        return parenthesize(expr.oper.lexeme, exprs, *this);
    }
};