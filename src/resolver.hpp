#ifndef RESOLVER_H_ 
#define RESOLVER_H_ 

#include <stack>


#include "types.hpp"
#include "error.hpp"
#include "environment.hpp"
#include "interpreter.hpp"

enum class FunctionType {
    NONE, 
    FUNCTION
};

class Resolver: public ExprVisitor, public StmtVisitor{
public:
    FunctionType currentFunction = FunctionType::NONE;

    Resolver(Interpreter* interpreter) : interpreter(interpreter) {};

    Value* visitBinary(Binary& expr);
    Value* visitGrouping(Grouping& expr) ;
    Value* visitLiteral(Literal& expr) ;
    Value* visitUnary(Unary& expr) ;
    Value* visitVariable(Variable& expr);
    Value* visitAssign(Assign& expr);
    Value* visitLogicalExpr(Logical& expr);
    Value* visitCallExpr(Call& expr);

    void visitFunctionStmt(FunctionStmt& stmt);
    void visitExprStmt(ExprStmt& stmt);
    void visitPrintStmt(PrintStmt& stmt);
    void visitVarStmt(VarStmt& stmt);
    void visitBlockStmt(BlockStmt& stmt);
    void visitIfStmt(IfStmt& stmt);
    void visitWhileStmt(WhileStmt& stmt) ;
    void visitReturnStmt(ReturnStmt& stmt);

    void resolve(std::vector<Statement*> statements);

private: 

    Interpreter* interpreter;

     std::vector<std::unordered_map<std::string, bool>*> scopes; //stack

    void resolve(Statement* statement);
    void resolve(Expr* expr);
    void beginScope();
    void endScope();
    void declare(Token name);
    void define(Token name);
    void resolveLocal(Expr* expr, Token name);
    void resolveFunction(FunctionStmt& function, FunctionType ftype);


    
};


#endif //LOXFUNCTION_H_