#include <sstream> // For std::ostringstream

#include "types.h"


class RuntimeError: public std::runtime_error {
public:
    RuntimeError(Token token, const std::string& message) : std::runtime_error(message), token(token), message(message) {};
    Token token;
    std::string message;

};

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

public:
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


    void report(int line, const std::string& where, const std::string& message) {
        std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
        hadError = true;
    }

    void error(int line, const std::string& message) {
        report(line, "", message);
    }
};