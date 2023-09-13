#include "types.hpp"
#include "environment.hpp"

class Interpreter; 
class LoxFunction : public LoxCallable {
public:

    Value* call(Interpreter* interpreter, std::vector<Value*> arguments) ;
    int arity() { return declaration->params.size();};
    std::string toString() {return "<fn " + declaration->name.lexeme + ">" ;};

    LoxFunction(FunctionStmt& declaration) : declaration(&declaration) {};

private: 
    FunctionStmt* declaration;
    
};