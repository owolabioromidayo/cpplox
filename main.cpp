#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cctype>
#include <unordered_map>

#include "types.h"
#include "scanner.cpp"
#include "parser.cpp"
#include "pretty_printer.cpp"


void run(const std::string& source) {
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();
    Parser* parser = new Parser(tokens);
    Expr* expression = parser->parse();

    if (hadError) return;

    PrettyPrinter p;

    expression->accept(p) ;

    
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

    std::string line;
    while (std::getline(file, line)) {
        run(line);
        if (hadError){
            return;
        }
    }

    file.close();
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

