#include <iostream>
#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"

class A
{
public:
    virtual void a() { std::cout << "A-a\n"; };
    virtual void b() { std::cout << "A-b\n"; };
};

int main()
{
    std::string source =
        "while 0 == 1\n"
        "\tcontinue"
        ;

    Lexer lexer(&source);
    std::vector<Token> tokens = lexer.ScanTokens();

    /*for (Token token : tokens)
    {
        std::cout << token.literal << ' ' << (int)token.type << std::endl;
    }*/

    ExprBlock *block = ParseExprs(tokens);

    std::string compiled = Compile(block);

    std::cout << compiled;
}
