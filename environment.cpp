#include <unordered_map>
#include <string>
#include "types.h"
#include "error.h"

class Environment {
private:
    std::unordered_map<std::string, Value*> values; 


public:
    void define(std::string name, Value* value) {
       values[name] = value;

    }

    Value* get(Token name){
        auto it = values.find(name.lexeme);

        if (it != values.end()) 
           return it->second;

        std::ostringstream oss;
        oss << "Undefined variable " << name.lexeme << "."; 
        std::cout << "Undefined variable " << name.lexeme << "."; 
        throw new RuntimeError(name, oss.str());
    }

    void view(){
        for (auto it = values.begin(); it != values.end(); it++)
            std::cout << it->first << " " << it->second->view() << " \n";
    }

    void assign(Token name, Value* value){
        auto it = values.find(name.lexeme);
        if (it != values.end()){
            values[name.lexeme] = value;
            return;
        }

        throw new RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }
};