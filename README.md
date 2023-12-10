# cpplox
A C++ implementation of the tree-walking interpreter from [Crafting Interpreters](https://craftinginterpreters.com/).

## Usage:
```shell
git clone https://github.com/owolabioromidayo/cpplox
cd cpplox
g++ src/main.cpp -o lox
```
REPL Mode :`./lox`
<br />
Run a Lox file : `./lox filepath`


## Parser grammar

cpplox implements most of the [original Lox definition](https://craftinginterpreters.com/appendix-i.html) except classes and inheritance.
Here's the parser grammar:

```text
program      => declaration* EOF
declaration  => funcDecl | varDecl | statement
funDecl      => "fun" function
function     => IDENTIFIER "(" parameters? ")" block
parameters   => IDENTIFIER ( "," IDENTIFIER )*
varDecl      => "var" IDENTIFIER ( "=" expression )? ";"
statement    => exprStmt | ifStmt | forStmt | printStmt | returnStmt | whileStmt
                         | breakStmt | continueStmt | block
exprStmt     => expression ";"
ifStmt       => "if" "(" expression ")" statement ( "else" statement )?
forStmt      => "for" "(" ( varDecl | exprStmt | ";" ) expression? ";" expression? ")" statement
printStmt    => "print" expression ";"
returnStmt   => "return" expression? ";"
whileStmt    => "while" "(" expression ")" statement
block        => "{" declaration* "}" ;
expression   => series
series       => assignment ( "," assignment )*
assignment   => ( call "." )? IDENTIFIER "=" assignment | ternary
ternary      => logic_or ( "?" ternary ":" ternary )*
logic_or     => logic_and ( "or" logic_and )*
logic_and    => equality ( "and" equality )*
equality     => comparison ( ( "!=" | "==" ) comparison )
comparison   => term ( ( ">" | ">=" | "<" | "<=" ) term )*
term         => factor ( ( "+" | "-" ) factor )*
factor       => unary ( ( "/" | "*" ) unary )*
unary        => ( "!" | "-" ) unary | call
call         => primary ( "(" arguments? ")" | "." IDENTIFIER )*
arguments    => expression ( "," expression )*
primary      => NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")"
                                | IDENTIFIER | functionExpr | "super" . IDENTIFIER
functionExpr => "fun" IDENTIFIER? "(" parameters? ")" block
```





