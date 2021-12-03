#pragma once
#include <string>

class ASMRunner
{
private:
	std::string dir;
	std::string projectName;

public:
	ASMRunner(std::string dir, std::string projectName);
	void Execute();
};
