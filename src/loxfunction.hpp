#ifndef LOXFUNCTION_H_
#define LOXFUNCTION_H_

#include "types.hpp"
#include "error.hpp"
#include "environment.hpp"
#include "interpreter.hpp"

class LoxFunction : public LoxCallable {
public:

    Value* call(Interpreter* interpreter, std::vector<Value*> arguments) ;
    int arity() { return declaration->params.size();};
    std::string toString() {return "<fn " + declaration->name.lexeme + ">" ;};

    LoxFunction(FunctionStmt& declaration, Environment* closure) : declaration(&declaration), closure(closure) {};

private: 
    FunctionStmt* declaration;
    Environment* closure;
    
};


#endif //LOXFUNCTION_H_