#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cctype>
#include <unordered_map>

// #include "types.cpp"


class Scanner {
public:
    Scanner(const std::string& source) : source(source) {}

    std::vector<Token> scanTokens() {
        while (!isAtEnd()){ 
            start = current;
            scanToken();
        }

        addToken(TokenType::EOF_);
        return tokens;
    }

    

private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0; 
    int current = 0;
    int line = 1;

    bool isAtEnd(){
        return current >= source.length();
    }

    void scanToken(){
        char c = advance();
        switch (c) {
            case '(': addToken(TokenType::LEFT_PAREN); break;
            case ')': addToken(TokenType::RIGHT_PAREN); break;
            case '{': addToken(TokenType::LEFT_BRACE); break;
            case '}': addToken(TokenType::RIGHT_BRACE); break;
            case ',': addToken(TokenType::COMMA); break;
            case '.': addToken(TokenType::DOT); break;
            case '-': addToken(TokenType::MINUS); break;
            case '+': addToken(TokenType::PLUS); break;
            case ';': addToken(TokenType::SEMICOLON); break;
            case '*': addToken(TokenType::STAR); break;

            case '!':
                addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
                break;
            case '=':
                addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
                break;
            case '<':
                addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
                break;
            case '>':
                addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
                break;

            case '/':
                if (match('/')) {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if(match('*')) {
                    //block comment start
                    while (peek() != '*' && !isAtEnd()) advance();
                    if(match('*') && match('/')) { break;}
                    else {
                        error(line, "Invalid block comment.");
                    } 
                } 
                else {
                    addToken(TokenType::SLASH);
                }
                break;


            // Ignore whitespace.
            case ' ':
            case '\r':
            case '\t':
                break;
            case '\n':
                line++;
                break;

            case '"': string(); break;


            default:
                if (std::isdigit(c)) {
                    number();
                }  else if (std::isalpha(c)) {
                    identifier();
                } else{
                    error(line, "Unexepected character.");
                }
                break; 
        }
    }


    void identifier() {
        while (std::isalnum(peek())) advance();
        std::string text = source.substr(start, current-start);
        TokenType type; 

        auto it = keywords.find(text);
        if (it != keywords.end()) {
            type = it->second; // Keyword found in the map
        } else {
            type = TokenType::IDENTIFIER; // Default to IDENTIFIER
        }

        addToken(type);

    }
    void number() {
        while (std::isdigit(peek())) advance();
        // Look for a fractional part.
        if (peek() == '.' && std::isdigit(peekNext())) {
        // Consume the "."
        advance();
        while (std::isdigit(peek())) advance();
        }
        addToken(TokenType::NUMBER, source.substr(start, current-start));
    }

    void string() {
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') line++;
            advance();
        }
        if (isAtEnd()) {
            error(line, "Unterminated string.");
            return;
        }

        // The closing ".
        advance();

        std::string value = source.substr(start + 1, (current - 1) - (start + 1));
        addToken(TokenType::STRING, value);
    }

    char peek() {
        if (isAtEnd()) return '\0';
        return source[current];
    }

    char peekNext() {
        if (current + 1 >= source.length()) return '\0';
        return source[current + 1];
    } 


    bool match(char expected) {
        if (isAtEnd()) return false;
        if (source[current] != expected) return false;
        current++;
        return true;
    }

    char advance(){
        current++;
        return source[current - 1];
    }

    // Function to add a token with no literal
    void addToken(TokenType type) {
        addToken(type, "NULL"); 
    }
    // Function to add a token with a literal
    void addToken(TokenType type, std::string literal){
        std::string text = source.substr(start, current - start);
        tokens.push_back(Token(type, text, literal, line));
    }
};
