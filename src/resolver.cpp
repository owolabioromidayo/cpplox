#include <sstream> 
#include <chrono>

#include "resolver.hpp"


void Resolver::resolve(std::vector<Statement*> statements){
    for(Statement* statement : statements){
        resolve(statement);
    }
}

void Resolver::resolve(Statement* statement){
    statement->accept(*this);
}

void Resolver::resolve(Expr* expr){
    expr->accept(*this);
}

void Resolver::beginScope(){
    scopes.push_back(new std::unordered_map<std::string, bool> ); 
}

void Resolver::endScope(){
    scopes.pop_back();
}

void Resolver::declare(Token name) {
 if (scopes.size() == 0) return;
    std::unordered_map<std::string, bool>* scope = scopes[scopes.size() -1];
    if (scope->find(name.lexeme) != scope->end()){
        error(name.line, "Already variable with this name in this scope");
    }
    (*scope)[name.lexeme] = false;
 }

void Resolver::define(Token name) {
 if (scopes.size() == 0) return;
    std::unordered_map<std::string, bool>* scope = scopes[scopes.size() -1];
    (*scope)[name.lexeme] = true;
 }


void Resolver::resolveLocal(Expr* expr, Token name){
    for (int i = scopes.size() - 1; i >= 0; i--) {
        if (scopes[i]->find(name.lexeme) != scopes[i]->end()){
            interpreter->resolve(expr, scopes.size() - i - 1);
            return;
        }
    }
}

void Resolver::resolveFunction(FunctionStmt& function, FunctionType ftype){
    FunctionType enclosingFunction = currentFunction;
    currentFunction = ftype;
    beginScope();
    for (Token param : function.params){
        declare(param);
        define(param);

    }
    resolve(function.body);
    endScope();

    currentFunction = enclosingFunction;
}





void Resolver::visitVarStmt(VarStmt& stmt) {
    declare(stmt.name);
    if (stmt.initializer != nullptr) {
        resolve(stmt.initializer);
    }
    define(stmt.name);
    return;
 }


void Resolver::visitBlockStmt(BlockStmt& stmt){
    beginScope();
    resolve(stmt.statements);
    endScope();
    return; 
}


void Resolver::visitFunctionStmt(FunctionStmt& stmt){
    declare(stmt.name);
    define(stmt.name);

    resolveFunction(stmt, FunctionType::FUNCTION);
    return;

}

void Resolver::visitExprStmt(ExprStmt& stmt){
    resolve(stmt.expression);
    return;
}

void Resolver::visitIfStmt(IfStmt& stmt){
    resolve(stmt.condition);
    resolve(stmt.thenBranch);
    if(stmt.elseBranch != nullptr) resolve(stmt.elseBranch);
    return;
}

void Resolver::visitPrintStmt(PrintStmt& stmt){
    resolve(stmt.expression);
    return; 
}

void Resolver::visitReturnStmt(ReturnStmt& stmt){
    if (currentFunction == FunctionType::NONE){
        error(stmt.keyword.line, "Cant return from top level code");
    }
    if(stmt.value != nullptr){
        resolve(stmt.value);
    }
    return;
}

void Resolver::visitWhileStmt(WhileStmt& stmt){
    resolve(stmt.condition);
    resolve(stmt.body);
    return;
}

Value* Resolver::visitBinary(Binary& expr){ 
    resolve(&expr.left);
    resolve(&expr.right);
    return new Value();
}


Value* Resolver::visitCallExpr(Call& expr){ 
    resolve(expr.callee);
    for (Expr*  argument: expr.arguments){
        resolve(argument); 
    }
    return new Value();
}
 
Value* Resolver::visitGrouping(Grouping& expr){
    resolve(&expr.expression);
    return new Value(); 
}

Value* Resolver::visitLiteral(Literal& expr){
    return new Value();
}

Value* Resolver::visitLogicalExpr(Logical& expr){
    resolve(expr.left);
    resolve(expr.right);
    return new Value();
}

Value* Resolver::visitUnary(Unary& expr) {
    resolve(&expr.right);
    return new Value();
 }


Value* Resolver::visitVariable(Variable& expr){ 


    if (!(scopes.size() == 0)){ 
        auto temp = (*scopes[scopes.size() -1]);
        if (temp.find(expr.name.lexeme) != temp.end()){
            if (temp[expr.name.lexeme] == false){
                error(expr.name.line,
                "Can't read local variable in its own initializer.");
            }
        }
    // if (!(scopes.size() == 0) && (*scopes[scopes.size() -1]).at(expr.name.lexeme) == false) {
    }
    resolveLocal(&expr, expr.name);
    return new Value();
}

Value* Resolver::visitAssign(Assign& expr){
    resolve(expr.value);
    resolveLocal(&expr, expr.name);
    return new Value(); 
}