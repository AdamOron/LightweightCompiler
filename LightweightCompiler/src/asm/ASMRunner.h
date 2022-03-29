#pragma once
#include <string>

class ASMRunner
{
private:
	const std::string &outputDir;
	const std::string &projectName;

public:
	ASMRunner(const std::string &outputDir, const std::string &projectName);
	void Execute();
};
