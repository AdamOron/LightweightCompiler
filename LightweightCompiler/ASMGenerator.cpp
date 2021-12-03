#include "ASMGenerator.h"

const std::string ASMGenerator::LABEL_PREFIX = "L";

std::string ASMGenerator::CreateLabel(const size_t labelIndex)
{
	return LABEL_PREFIX + std::to_string(labelIndex);
}

ASMGenerator::ASMGenerator()
{
	this->code = "";
	this->labelCount = 0;
}

ASMGenerator *ASMGenerator::instance = NULL;

ASMGenerator *ASMGenerator::GetInstance()
{
	if (instance == NULL)
	{
		instance = new ASMGenerator();
	}

	return instance;
}

std::string GetReg(const ASMReg reg)
{
	switch (reg)
	{
	case ASMReg::EAX:
		return "eax";

	case ASMReg::EBX:
		return "ebx";

	case ASMReg::ECX:
		return "ecx";

	case ASMReg::EDX:
		return "edx";

	default:
		return "Unimplemented Register";
	}
}

std::string GetInstr(const ASMInstr instr)
{
	switch (instr)
	{
	case ASMInstr::MOV:
		return "MOV";

	case ASMInstr::PUSH:
		return "PUSH";

	case ASMInstr::POP:
		return "POP";

	case ASMInstr::ADD:
		return "ADD";

	case ASMInstr::SUB:
		return "SUB";

	case ASMInstr::IMUL:
		return "IMUL";

	case ASMInstr::IDIV:
		return "IDIV";

	case ASMInstr::AND:
		return "AND";

	case ASMInstr::OR:
		return "OR";

	case ASMInstr::NEG:
		return "NEG";

	case ASMInstr::NOT:
		return "NOT";

	default:
		return "Unimplemented Instruction";
	}
}

void ASMGenerator::Append(const std::string code)
{
	this->code += code;
}

void ASMGenerator::AppendLine(const std::string code)
{
	Append(code + '\n');
}

void ASMGenerator::AppendSpace()
{
	AppendLine("");
}

void ASMGenerator::AppendComment(const std::string comment)
{
	AppendLine(";; " + comment);
}

std::string ASMGenerator::GenerateLabel()
{
	return CreateLabel(labelCount++);
}

int ASMGenerator::LabelCount() const
{
	return labelCount;
}

void ASMGenerator::PushValue(const std::string value)
{
	AppendLine("PUSH " + value);
}

void ASMGenerator::PopValue(const ASMReg reg)
{
	AppendLine("POP " + GetReg(reg));
}

void ASMGenerator::AppendUnary(const ASMInstr instr)
{
	/* Avoid using EAX in unary instructions; a lot of them use EAX implicitly */
	AppendLine("POP edx");
	AppendLine(GetInstr(instr) + " edx");
	AppendLine("PUSH edx");
}

void ASMGenerator::AppendBinary(const ASMInstr instr)
{
	AppendLine("POP eax");
	AppendLine("POP ebx");
	AppendLine(GetInstr(instr) + " eax, ebx");
	AppendLine("PUSH eax");
}

void ASMGenerator::EnterLoop()
{
	std::string loopLabel = CreateLabel(labelCount++);

	AppendComment("-------- Entering Loop --------");
	AppendLine("POP ecx");
	AppendLine(loopLabel + ":");
}

void ASMGenerator::ExitLoop()
{
	std::string loopLabel = CreateLabel(labelCount - 1);

	AppendLine("LOOP " + loopLabel);
	AppendComment("----------------------------------------");
}

void ASMGenerator::EnterMethod()
{
	AppendComment("Method Prologue");
	AppendLine("PUSH ebp");
	AppendLine("MOV ebp, esp");
	AppendSpace();
}

void ASMGenerator::ExitMethod()
{
	AppendComment("Method Epilogue");
	AppendLine("MOV esp, ebp");
	AppendLine("POP ebp");
	//AppendLine("RET");
	AppendSpace();
}

void ASMGenerator::FilePrologue()
{
	AppendLine("include \\masm32\\include\\masm32rt.inc");
	AppendSpace();
	AppendLine(".code");
	AppendLine("start:");
	AppendSpace();
}

void ASMGenerator::FileEpilogue()
{
	AppendSpace();
	AppendLine("end start");
}
