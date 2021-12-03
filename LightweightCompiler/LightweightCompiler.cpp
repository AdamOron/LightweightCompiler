#include <iostream>
#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"
#include "ASMRunner.h"
#include <chrono>
#include <fstream>

void CompileAndExecute(std::string *source)
{
    // Begin bench
    auto start = std::chrono::high_resolution_clock::now();

    std::string fname = "F:\\Workspace\\VisualStudio\\C++\\LightweightCompilerRefactor\\LightweightCompiler\\Test\\result";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.ScanTokens();

    ExprBlock *block = ParseExprs(tokens);

    //std::cout << *exprs << std::endl;

    std::string compiled = Compile(block);

    // End bench
    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "\nCompilation Time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() << "ns\n";

    //std::cout << compiled;

    std::ofstream file(fname + ".asm");
    file << compiled;
    file.close();

    ASMRunner runner("F:\\Workspace\\VisualStudio\\C++\\LightweightCompilerRefactor\\LightweightCompiler\\Test\\", "result");
    runner.Execute();
}

std::string ReadFile(std::string filePath)
{
    std::string source;

    std::ifstream sourceFile(filePath);

    std::string curLine;
    while (std::getline(sourceFile, curLine))
    {
        source += curLine + '\n';
    }

    sourceFile.close();

    return source;
}

int main()
{
    std::string source = ReadFile("F:\\Workspace\\VisualStudio\\C++\\LightweightCompilerRefactor\\LightweightCompiler\\Test\\source.txt");
    
    CompileAndExecute(&source);
}
