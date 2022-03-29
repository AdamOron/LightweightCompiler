#include "Compiler.h"

void ThrowCompileError(std::string error)
{
	std::cerr << "Compiler Error: " << error;
	exit(1);
}

std::string Compile(const ExprGroup *block)
{
	StatementVisitor visitor;

	visitor.asmGen->FilePrologue();
	visitor.asmGen->EnterMethod();

	visitor.Visit(block);

	visitor.asmGen->ExitMethod();
	visitor.asmGen->FileEpilogue();

	return visitor.asmGen->code;
}