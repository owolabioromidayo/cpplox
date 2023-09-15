#include "loxfunction.hpp"


Value* LoxFunction::call(Interpreter* interpreter, std::vector<Value*> arguments) {
    Environment* environment = new Environment(interpreter->globals);
    
    for (int i = 0; i < declaration->params.size(); i++) {
        environment->define(declaration->params.at(i).lexeme,
        arguments.at(i));
    }
    try{ 
        interpreter->executeBlock(declaration->body, environment);
    } catch(Return returnValue){
        return returnValue.value;
    }
    return new Value();
}