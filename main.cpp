#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cctype>
#include <unordered_map>

#include "types.hpp"
#include "error.hpp"
#include "loxfunction.h"

#include "scanner.cpp"
#include "parser.cpp"
#include "interpreter.cpp"

Interpreter* interpreter = new Interpreter();





Value* LoxFunction::call(Interpreter* interpreter, std::vector<Value*> arguments) {
    Environment* environment = new Environment(interpreter->globals);
    
    for (int i = 0; i < declaration->params.size(); i++) {
        environment->define(declaration->params.at(i).lexeme,
        arguments.at(i));
    }
    interpreter->executeBlock(declaration->body, environment);
    return new Value();
}

void run(const std::string& source) {
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();
    Parser* parser = new Parser(tokens);
    std::vector<Statement*> statements = parser->parse();
    interpreter->interpret(statements);

    // if (hadError) return;

    // PrettyPrinter p;

    // expression->accept(p);
    // std::cout << std::endl;

    
    // for (const Token& token : tokens) {
    //     std::cout << token.lexeme << std::endl;
    // }
}


void runFile(char* path){

    std::ifstream file;
    file.open(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << path << std::endl;
        return;
    }

    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the entire file content into a string
    std::string fileContents;
    fileContents.resize(fileSize);
    file.read(&fileContents[0], fileSize);
    file.close();

    run(fileContents);
}

void runPrompt() {
    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);

        if (std::cin.eof()) {
            // Exit the loop on Ctrl+D or EOF
            break;
        }

        run(line);
        hadError = false;
    }
}

int main(int argc, char* argv[]){

    if (argc > 2){
        std::cout << "Usage: cpplox [script] \n";
        return 0;
    } else if (argc == 2){
        runFile(argv[1]);
    } else{
        runPrompt();
    }

    // Token minusToken(TokenType::MINUS, "-", "", 1);
    // Token starToken(TokenType::STAR, "*", "", 1);

    // Literal literal123("123", TokenType::NUMBER);
    // Literal literal45dot67("45.67", TokenType::NUMBER);

    // Unary unaryMinus(minusToken, literal123);
    // Binary expression(unaryMinus, starToken, literal45dot67);

    // PrettyPrinter p;
    // // p.visitUnary(unaryMinus);
    // p.visitBinary(expression);

}

