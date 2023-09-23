#ifndef INTERPRETER_H__
#define INTERPRETER_H__

#include "types.hpp"
#include "error.hpp"
#include "pretty_printer.hpp"
#include "environment.hpp"
#include "loxfunction.hpp"
#include "clockcallable.hpp"


class Interpreter: public ExprVisitor, public StmtVisitor {

private: 
    Value* evaluate(Expr* expr);
    bool isTruthy(Value* value);
    bool isEqual(Value* a, Value* b);
    void execute(Statement* stmt);

public:

    Environment* globals = new Environment();
    Environment* environment = new Environment(globals);


     std::unordered_map<Expr*, int> locals; //stack

    ~Interpreter();
    Interpreter();

    void executeBlock(std::vector<Statement*> statements, Environment* environment) ;
    void interpret(std::vector<Statement*> statements);
    void resolve(Expr* expr, int depth);
    Value* lookUpVariable(Token name, Expr* expr);

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

};

#endif //INTERPRETER_H__