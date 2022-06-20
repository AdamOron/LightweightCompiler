#pragma once
#include <string>

enum class ASMReg
{
	EAX,
	EBX,
	ECX,
	EDX,
};

std::string GetReg(ASMReg reg);

enum class ASMInstr
{
	MOV,
	PUSH,
	POP,

	ADD,
	SUB,
	IMUL,
	IDIV,
	AND,
	OR,

	NEG,
	NOT,
};

std::string GetInstr(ASMInstr instr);

class ASMGenerator
{
private:
	static ASMGenerator *instance;
	static const std::string LABEL_PREFIX;
	size_t labelCount;
	ASMGenerator();
public:
	static ASMGenerator *GetInstance();

	static std::string CreateLabel(const size_t labelIndex);

	std::string code;

	void Append(const std::string code);
	void AppendLine(const std::string code);
	void AppendSpace();

	void AppendComment(const std::string comment);

	std::string GenerateLabel();
	int LabelCount() const;

	void PushValue(const std::string value);
	void PopValue(const ASMReg reg);

	void AppendUnary(const ASMInstr instr);
	void AppendBinary(const ASMInstr instr);

	void EnterLoop();
	void ExitLoop();

	void EnterMethod();
	void ExitMethod();

	void FilePrologue();
	void FileEpilogue();
};
