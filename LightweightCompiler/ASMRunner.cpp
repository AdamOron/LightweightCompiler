#include "ASMRunner.h"
#include <iostream>
#include <chrono>

ASMRunner::ASMRunner(std::string dir, std::string projectName) :
	dir(dir),
	projectName(projectName)
{
}

void ASMRunner::Execute()
{
	std::string extLessFile = dir + projectName;

	std::string asmFile = extLessFile + ".asm";
	std::string objFile = extLessFile + ".obj";
	std::string exeFile = extLessFile + ".exe";

	std::string asmToObj = "\\masm32\\bin\\ml /Fo " + objFile + " /c /Zd /coff " + asmFile;
	std::string objToExe = "\\masm32\\bin\\Link /SUBSYSTEM:CONSOLE /OUT:" + exeFile + " " + objFile;
	std::string execExe = exeFile;

	//std::string compileCommand = "C: & " + asmToObj + " > nul & " + objToExe + " > nul"; // Silent mode
	std::string compileCommand = "C: & " + asmToObj + " & " + objToExe;
	system(compileCommand.c_str());

	std::cout << "Output:\n";

	auto start = std::chrono::high_resolution_clock::now();
	system(execExe.c_str());
	auto finish = std::chrono::high_resolution_clock::now();
	std::cout << "\nExecution Time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() << "ns\n";
}
