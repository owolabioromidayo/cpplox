#include "loxfunction.hpp"


Value* LoxFunction::call(Interpreter* interpreter, std::vector<Value*> arguments) {
    Environment* environment = new Environment(interpreter->environment);
    Environment* previous = interpreter->environment;
    interpreter->environment = environment;
    
    for (int i = 0; i < declaration->params.size(); i++) {
        environment->define(declaration->params.at(i).lexeme, arguments.at(i));

    }
    try{ 
        interpreter->executeBlock(declaration->body, environment);
    } catch(Return returnValue){
        // for (int i = 0; i < declaration->params.size(); i++) 
        //     std::cout <<  declaration->params.at(i).lexeme << " " << arguments.at(i)->view() << "\t";

        // std::cout << this->toString() << " " << returnValue.value->view() << std::endl;

        interpreter->environment = previous;
        return returnValue.value;
    }
    interpreter->environment = previous;
    return new Value();
}