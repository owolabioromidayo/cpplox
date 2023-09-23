#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_
#include <unordered_map>
#include <string>
#include "types.hpp"
#include "error.hpp"

class Environment {
private:

    std::unordered_map<std::string, Value*> values; 


public:
    Environment(Environment* enclosing) : enclosing(enclosing){}
    
    Environment(){
        enclosing = nullptr;
    }

    Environment* enclosing;

    Environment* ancestor( int distance){
        Environment* environment = this; 
        for(int i=0; i < distance; i++){ 
            environment = environment->enclosing; 
        }

        return environment;
    }

    void define(std::string name, Value* value) {
       values[name] = value;

    }


    Value* get(Token name){
        auto it = values.find(name.lexeme);

        if (it != values.end()) 
           return it->second;

        if (enclosing != nullptr) 
            return enclosing->get(name);

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    Value* getAt(int distance, Token name){
        auto temp = ancestor(distance)->values; 
        if (temp.find(name.lexeme) != temp.end())
            return temp[name.lexeme];

        throw RuntimeError(name, "Undefined variable '" + name.lexeme +"'.");
        return new Value(); 
    }

    void view(){
        for (auto it = values.begin(); it != values.end(); it ++)
            std::cout << it->first << " " << it->second->view() << " \n";
    }

    void assign(Token name, Value* value){
        auto it = values.find(name.lexeme);
        if (it != values.end()){
            values[name.lexeme] = value;
            return;
        }

        if (enclosing != nullptr) 
            return enclosing->assign(name, value);

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    void assignAt(int distance, Token name, Value* value) {
        ancestor(distance)->values[name.lexeme] = value;
    }

};


#endif //ENVIRONMENT_HPP_