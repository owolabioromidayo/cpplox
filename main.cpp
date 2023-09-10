#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cctype>
#include <unordered_map>


bool hadError = false; 

// we have NULL, string, double(nuumber) literals. we can always specify the fromstr conversion later

enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    // Literals.
    IDENTIFIER, STRING, NUMBER,
    // Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,
    EOF_, 

    //block comment
    BLOCK_COMMENT_START, BLOCK_COMMENT_END
};

std::unordered_map<std::string, TokenType> keywords = {
    {"and", TokenType::AND},
    {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},
    {"false", TokenType::FALSE},
    {"for", TokenType::FOR},
    {"fun", TokenType::FUN},
    {"if", TokenType::IF},
    {"nil", TokenType::NIL},
    {"or", TokenType::OR},
    {"print", TokenType::PRINT},
    {"return", TokenType::RETURN},
    {"super", TokenType::SUPER},
    {"this", TokenType::THIS},
    {"true", TokenType::TRUE},
    {"var", TokenType::VAR},
    {"while", TokenType::WHILE}
};

class Token {
public:
    TokenType type;
    std::string lexeme;
    std::string literal;
    int line;

    //literal handling seems very broken, thanks cpp and no reflection

    Token(TokenType type, const std::string& lexeme, const std::string& literal, int line) :
        type(type), lexeme(lexeme), literal(literal), line(line) {}

    std::string toString() {
        return typeid(type).name() + std::string(" ") + lexeme + " " + literal;
    }
};


void report(int line, const std::string& where, const std::string& message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
    hadError = true;
}

void error(int line, const std::string& message) {
    report(line, "", message);
}

class Scanner {
public:
    Scanner(const std::string& source) : source(source) {}

    std::vector<Token> scanTokens() {
        while (!isAtEnd()){ 
            start = current;
            scanToken();
        }

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


            case ' ':
            case '\r':
            case '\t':
            // Ignore whitespace.
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






void run(const std::string& source) {
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();
    
    for (const Token& token : tokens) {
        std::cout << token.lexeme << std::endl;
    }
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

 }

