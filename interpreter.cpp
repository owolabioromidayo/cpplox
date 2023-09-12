#include <sstream> // For std::ostringstream

#include "types.h"
#include "error.h"

#include "environment.cpp"

class Interpreter: public ExprVisitor, public StmtVisitor {

private: 
    Value* evaluate(Expr* expr){
        return expr->accept(*this);
    }

    Value* isTruthy(Value* value){
        if(value->type == ValueType::NIL)
            return new Value(false);
        if (value->type == ValueType::BOOLEAN)
            return value;

        return new Value(true);
    }

    Value* isEqual(Value* a, Value* b){
        if (a->type == ValueType::NIL && b->type == ValueType::NIL)
            return new Value(true);
        if (a->type == ValueType::NIL) 
            return new Value(false);
        
        return new Value( a->bool_ == b->bool_ );
    }

    void execute(Statement* stmt){
        stmt->accept(*this);
    }

    void executeBlock(std::vector<Statement*> statements, Environment* environment) {
        Environment* previous = this->environment;
        this->environment = environment;
        
            for (Statement* statement: statements){
                    execute(statement); 
            }

        this->environment = previous;
    }

    Environment* environment = new Environment();

public:
    ~Interpreter(){}

    Value* visitBinary(Binary& expr) {
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

                case TokenType::BANG_EQUAL: return new Value(!( isEqual(left, right)->bool_ ));
                case TokenType::EQUAL_EQUAL: return isEqual(left, right);
            }
        } else if(left->type == ValueType::STRING && right->type == ValueType::STRING){
           switch (expr.oper.type){
                case TokenType::PLUS:
                    return new Value(left->str + right->str);

                case TokenType::BANG_EQUAL: return new Value(!( isEqual(left, right)->bool_ ));
                case TokenType::EQUAL_EQUAL: return isEqual(left, right);
           }
        } 

        //if it didnt match into anything
        std::ostringstream oss;
        oss << expr.oper.toString() << " " << "Operation failed on types \n";
        throw new RuntimeError(expr.oper, oss.str());
                
    }
    

    Value* visitGrouping(Grouping& expr) {
        return evaluate(&expr.expression);
    }

    Value* visitLiteral(Literal& expr) {
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


    Value* visitUnary(Unary& expr) {

        Value* right = evaluate(&expr.right);
        switch(expr.oper.type){
            case TokenType::MINUS :
                return new Value(-right->number);
            case TokenType::BANG:
                return new Value(!isTruthy(right));
        }

        return new Value(); //NIL

    }

    Value* visitVariable(Variable& expr){ 
        return environment->get(expr.name); 
    }

    Value* visitAssign(Assign& expr){
        Value* value = evaluate(expr.value);
        environment->assign(expr.name, value);
        return value; 
    }


    void visitExprStmt(ExprStmt& stmt) {
        evaluate(stmt.expression);
        return;
    }

    void visitPrintStmt(PrintStmt& stmt) {
        Value* value = evaluate(stmt.expression);
        std::cout << value->view() << std::endl;
        return;
    }

    void interpret(std::vector<Statement*> statements){
        try{
            for (Statement* statement: statements){
                execute(statement);
            }
        } catch(RuntimeError err) {
            error(err.token.line, err.message);
        }
    }

    void visitVarStmt(VarStmt& stmt){
        Value* value = new Value();
        if (stmt.initializer != nullptr) 
            value = evaluate(stmt.initializer); 
        
        environment->define(stmt.name.lexeme, value);
        return; 
    }

    void visitBlockStmt(BlockStmt& stmt){
        executeBlock(stmt.statements, new Environment(environment));
        return; 
    }
};