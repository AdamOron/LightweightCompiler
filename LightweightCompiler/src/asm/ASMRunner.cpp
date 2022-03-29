#include "ASMRunner.h"
#include <iostream>
#include <chrono>

ASMRunner::ASMRunner(const std::string &outputDir, const std::string &projectName) :
	outputDir(outputDir),
	projectName(projectName)
{
}

void ASMRunner::Execute()
{
	std::string asmPath = outputDir + projectName + ".asm";
	std::string objPath = outputDir + projectName + ".obj";
	std::string exePath = outputDir + projectName + ".exe";

	/* Compile ASM code to OBJ, then compile OBJ to EXE */
	std::string compile = "nasm -fwin32 " + asmPath + " & gcc " + objPath + " -o " + exePath;
	/* Execute command */
	system(compile.c_str());

	std::cout << "Output:\n";

	/* Begin EXE runtime benchmark */
	auto start = std::chrono::high_resolution_clock::now();
	/* Execute EXE file */
	system(exePath.c_str());
	/* End EXE runtime benchmark & print result. Kinda useless benchmark */
	auto finish = std::chrono::high_resolution_clock::now();
	std::cout << "\nExecution Time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() << "ns\n";
}
