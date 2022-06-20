#include <iostream>
#include "tokens/Tokenizer.h"
#include "Parser/Parser.h"
#include "compiler/Compiler.h"
#include "asm/ASMRunner.h"
#include <chrono>
#include <fstream>

std::string ReadFile(std::string filePath)
{
    std::string source;

    std::ifstream sourceFile(filePath);

    std::string curLine;
    while (std::getline(sourceFile, curLine))
        source += curLine + '\n';

    sourceFile.close();

    return source;
}

void CompileAndExecute(const std::string &sourceDir, const std::string &outputDir, const std::string &projectName)
{
    /* Read source file content */
    std::string sourceContent = ReadFile(sourceDir + projectName + ".txt");

    /* Create path to output Assembly file, expect it to end with .asm extension */
    std::string outputAsmPath = outputDir + projectName + ".asm";

    /* Begin compilation benchmark */
    auto compilationStart = std::chrono::high_resolution_clock::now();

    /* First stage: Scan Tokens  */
    Tokenizer tokenizer(sourceContent);
    std::vector<Token> tokens = tokenizer.ScanTokens();

    /* Second stage: Parse Tokens & Build AST */
    ExprGroup *block = ParseExprs(tokens);

    /* Third stage: Compile AST into ASM code */
    std::string compiled = Compile(block);

    /* End compilation benchmark & print result */
    auto compilationEnd  = std::chrono::high_resolution_clock::now();
    std::cout << "\nCompilation Time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(compilationEnd - compilationStart).count() << "ns\n";

    /* Write ASM code into output file */
    std::ofstream file(outputAsmPath);
    file << compiled;
    file.close();

    /* Run compiled ASM file */
    ASMRunner runner(outputDir, projectName);
    runner.Execute();
}

int main()
{
    std::string sourceDir = "E:\\Workspace\\VisualStudio\\C++\\LightweightCompilerRefactor\\TestProject\\";
    std::string outputDir = "E:\\Workspace\\VisualStudio\\C++\\LightweightCompilerRefactor\\TestProject\\out\\";
    std::string projectName = "example";
    
    CompileAndExecute(sourceDir, outputDir, projectName);
    /*ASMRunner runner(outputDir, projectName);
    runner.Execute();*/
}
