#include <sstream> 
#include <chrono>

#include "interpreter.hpp"




Value* Interpreter::evaluate(Expr* expr){
    return expr->accept(*this);
}

bool Interpreter::isTruthy(Value* value){
    if(value->type == ValueType::NIL)
        return false;
    if (value->type == ValueType::BOOLEAN){
        return value->bool_;
    }

    return  true;
}

bool Interpreter::isEqual(Value* a, Value* b){
    if (a->type == ValueType::NIL && b->type == ValueType::NIL)
        return true;
    if (a->type == ValueType::NIL) 
        return false;
    
    return  a->bool_ == b->bool_ ;
}

void Interpreter::execute(Statement* stmt){
    stmt->accept(*this);
}

Value* Interpreter::lookUpVariable(Token name, Expr* expr){
    auto distance = locals.find(expr); 
    if(distance != locals.end()){
        return environment->getAt(distance->second, name);
    }

    return globals->get(name); 
}

void Interpreter::resolve(Expr* expr, int depth){
    locals[expr] = depth;
}

Interpreter::~Interpreter(){}

Interpreter::Interpreter(){
    this->globals->define("clock", new Value(new ClockCallable()) );
}


void Interpreter::executeBlock(std::vector<Statement*> statements, Environment* environment) {
    Environment* previous = this->environment;
    this->environment = environment;

        for (Statement* statement: statements){
                execute(statement); 
        }

    this->environment = previous;
}

void Interpreter::interpret(std::vector<Statement*> statements){
    try{
        for (Statement* statement: statements){
            execute(statement);
        }
    } catch(RuntimeError err) {
        error(err.token.line, err.message);
    }
}

Value* Interpreter::visitBinary(Binary& expr) {
    Value* left = evaluate(&expr.left);        
    Value* right = evaluate(&expr.right);        

    //switch based on the type too
    if(left->type == ValueType::NUMBER && right->type == ValueType::NUMBER){
        switch (expr.oper.type) {
            case TokenType::GREATER:
                return new Value(left->number > right->number);
            case TokenType::GREATER_EQUAL:
                return new Value(left->number >= right->number);
            case TokenType::LESS:
                return new Value(left->number < right->number);
            case TokenType::LESS_EQUAL:
                return new Value(left->number <= right->number);
            case TokenType::MINUS:
                return new Value(left->number - right->number);
            case TokenType::SLASH:
                return new Value(left->number / right->number);
            case TokenType::STAR:
                return new Value(left->number * right->number);
            case TokenType::PLUS:
                return new Value(left->number + right->number);

            case TokenType::BANG_EQUAL: return new Value(!(isEqual(left, right)));
            case TokenType::EQUAL_EQUAL: return new Value(isEqual(left, right));
        }
    } else if(left->type == ValueType::STRING && right->type == ValueType::STRING){
        switch (expr.oper.type){
            case TokenType::PLUS:
                return new Value(left->str + right->str);

            case TokenType::BANG_EQUAL: return new Value(!( isEqual(left, right)));
            case TokenType::EQUAL_EQUAL: return new Value(isEqual(left, right));
        }
    } 

    //if it didnt match into anything
    std::ostringstream oss;
    oss << expr.oper.toString() << " " << "Operation failed on types \n";
    throw RuntimeError(expr.oper, oss.str());
            
}


Value* Interpreter::visitGrouping(Grouping& expr) {
    return evaluate(&expr.expression);
}

Value* Interpreter::visitLiteral(Literal& expr) {
    //just conv from token to Value for now.
    switch (expr.type){
        case TokenType::NUMBER:
            return new Value(std::stod(expr.value)); 
        case TokenType::STRING:
            return new Value(expr.value);
        case TokenType::TRUE:
            return new Value(true);
        case TokenType::FALSE:
            return new Value(false);
    }
    
    return new Value(); //NIL
}


Value* Interpreter::visitUnary(Unary& expr) {
    Value* right = evaluate(&expr.right);
    switch(expr.oper.type){
        case TokenType::MINUS :
            return new Value(-right->number);
        case TokenType::BANG:
            return new Value(!isTruthy(right));
    }

    return new Value(); //NIL

}

Value* Interpreter::visitVariable(Variable& expr){ 
    return lookUpVariable(expr.name, &expr);
}


Value* Interpreter::visitAssign(Assign& expr){
    Value* value = evaluate(expr.value);

    auto distance = locals.find(&expr);  
    if (distance != locals.end()){
        environment->assignAt(distance->second, expr.name, value);
    } else {
        globals->assign(expr.name, value);
    
    }
    return value; 
}

Value* Interpreter::visitLogicalExpr(Logical& expr){
    Value* left = evaluate(expr.left);

    if(expr.oper.type == TokenType::OR){ 
        if(isTruthy(left)) return left; 
    } else {
        if (!(isTruthy(left)) ) return left; 
    }
    return evaluate(expr.right);
}

Value* Interpreter::visitCallExpr(Call& expr){
    Value* callee = evaluate(expr.callee); //canat be value then, hmm
    std::vector<Value*> arguments;
    for (Expr* argument : expr.arguments) {
        arguments.push_back(evaluate(argument));
    }

    if (callee->type != ValueType::CALLABLE) { 
        throw RuntimeError(expr.paren,
        "Can only call functions and classes.");
    }

    LoxCallable* function = callee->callable;
    if (arguments.size() != function->arity()) {
        throw RuntimeError(expr.paren, "Expected " +
        std::to_string(function->arity()) + " arguments but got " +
        std::to_string(arguments.size()) + ".");
    }
    //definte the function
    // std::cout << expr.paren.toString() << std::endl; 
    this->globals->define(expr.paren.lexeme, new Value(function));
    // std::cout << this->environment->get(expr.paren)->view() << std::endl;
    return function->call(this, arguments);
}


void Interpreter::visitFunctionStmt(FunctionStmt& stmt){
    LoxFunction* function = new LoxFunction(stmt, this->environment);
    this->environment->define(stmt.name.lexeme, new Value(function));
    return;
} 

void Interpreter::visitExprStmt(ExprStmt& stmt) {
    evaluate(stmt.expression);
    return;
}

void Interpreter::visitPrintStmt(PrintStmt& stmt) {
    Value* value = evaluate(stmt.expression);
    std::cout << value->view() << std::endl;
    return;
}


void Interpreter::visitVarStmt(VarStmt& stmt){
    Value* value = new Value();
    if (stmt.initializer != nullptr){
        value = evaluate(stmt.initializer); 
        environment->define(stmt.name.lexeme, value);
        return;
    } 
        environment->define(stmt.name.lexeme, new Value());
}

void Interpreter::visitBlockStmt(BlockStmt& stmt){
    executeBlock(stmt.statements, new Environment(environment));
    return; 
}

void Interpreter::visitIfStmt(IfStmt& stmt){
    if (isTruthy(evaluate(stmt.condition))) {
        execute(stmt.thenBranch);
    } else if (stmt.elseBranch != nullptr) {
        execute(stmt.elseBranch);
    }
    return;
}

void Interpreter::visitWhileStmt(WhileStmt& stmt) {
    while (isTruthy(evaluate(stmt.condition))) 
        execute(stmt.body);

    return;
}

void Interpreter::visitReturnStmt(ReturnStmt& stmt){
    Value* value = new Value();
    if (stmt.value != nullptr) value = evaluate(stmt.value);
    throw Return(value);
}





