#include "../compiler/Compiler.h"

ValueVisitor::ValueVisitor(StatementVisitor *superVisitor) :
	ChildVisitor(superVisitor),
	returnType(NULL)
{
}

const Type *ValueVisitor::GetType()
{
	return returnType;
}

void ValueVisitor::Visit(const LitExpr *expr)
{
	if (expr->value->type == TokenType::BOOL)
	{
		superVisitor->asmGen->PushValue(expr->value->literal == Token::FALSE_LITERAL ? "0" : "1");
		returnType = TypeTable::TYPE_BOOL;
		return;
	}

	if (expr->value->type == TokenType::INT)
	{
		superVisitor->asmGen->PushValue(expr->value->literal);
		returnType = TypeTable::TYPE_INT;
		return;
	}

	ThrowCompileError("Invalid literal type");
}

void ValueVisitor::Visit(const AccessibleExpr *expr)
{
	Token *id = expr->id;
	Var *var = superVisitor->GetVar(id);

	if (var == NULL)
	{
		ThrowCompileError("Invalid accessor name " + id->literal);
	}

	superVisitor->asmGen->PushValue("DWORD [ebp-" + std::to_string(var->memOffset) + "]");

	returnType = var->type;
}

void ValueVisitor::AppendNot()
{
	std::string isFalse = superVisitor->asmGen->GenerateLabel();
	std::string exit = superVisitor->asmGen->GenerateLabel();

	superVisitor->asmGen->AppendLine("POP edx");

	superVisitor->asmGen->AppendLine("CMP edx, 0");
	superVisitor->asmGen->AppendLine("JZ " + isFalse);

	superVisitor->asmGen->AppendLine("PUSH 0");
	superVisitor->asmGen->AppendLine("JMP " + exit);
	superVisitor->asmGen->AppendLine(isFalse + ": PUSH 1");
	superVisitor->asmGen->AppendLine(exit + ":");
}

void ValueVisitor::Visit(const UnaryExpr *expr)
{
	expr->value->Accept(this);

	switch (expr->oper->type)
	{
	case TokenType::SUB:
		superVisitor->asmGen->AppendUnary(ASMInstr::NEG);
		break;

	case TokenType::NOT:
		AppendNot();
		break;
	}

	superVisitor->asmGen->AppendSpace();
}

void ValueVisitor::AppendAnd(const BinaryExpr *expr)
{
	std::string hasZero = superVisitor->asmGen->GenerateLabel();
	std::string exit = superVisitor->asmGen->GenerateLabel();

	superVisitor->asmGen->AppendLine("POP eax");
	superVisitor->asmGen->AppendLine("POP ebx");

	superVisitor->asmGen->AppendLine("CMP eax, 0");
	superVisitor->asmGen->AppendLine("JZ " + hasZero);
	superVisitor->asmGen->AppendLine("CMP ebx, 0");
	superVisitor->asmGen->AppendLine("JZ " + hasZero);

	superVisitor->asmGen->AppendLine("PUSH 1");
	superVisitor->asmGen->AppendLine("JMP " + exit);
	superVisitor->asmGen->AppendLine(hasZero + ": PUSH 0");
	superVisitor->asmGen->AppendLine(exit + ":");
}

void ValueVisitor::AppendOr(const BinaryExpr *expr)
{
	std::string hasOne = superVisitor->asmGen->GenerateLabel();
	std::string exit = superVisitor->asmGen->GenerateLabel();

	superVisitor->asmGen->AppendLine("POP eax");
	superVisitor->asmGen->AppendLine("POP ebx");

	superVisitor->asmGen->AppendLine("CMP eax, 0");
	superVisitor->asmGen->AppendLine("JNZ " + hasOne);
	superVisitor->asmGen->AppendLine("CMP ebx, 0");
	superVisitor->asmGen->AppendLine("JNZ " + hasOne);

	superVisitor->asmGen->AppendLine("PUSH 0");
	superVisitor->asmGen->AppendLine("JMP " + exit);
	superVisitor->asmGen->AppendLine(hasOne + ": PUSH 1");
	superVisitor->asmGen->AppendLine(exit + ":");
}

void ValueVisitor::AppendModulo()
{
	superVisitor->asmGen->AppendLine("POP eax");
	superVisitor->asmGen->AppendLine("MOV edx, 0");
	superVisitor->asmGen->AppendLine("POP ebx");
	superVisitor->asmGen->AppendLine("IDIV ebx");
	superVisitor->asmGen->AppendLine("PUSH edx");
}

void ValueVisitor::Visit(const BinaryExpr *expr)
{
	/* We push values from right to left due to ASM's LIFO stack */
	expr->right->Accept(this); // Push evaluated right expression onto the ASM stack
	const Type *right = returnType;
	expr->left->Accept(this); // Push evaluated left expression onto the ASM stack
	const Type *left = returnType;

	bool hasFloat = right == TypeTable::TYPE_FLOAT || left == TypeTable::TYPE_FLOAT;

	switch (expr->oper->type)
	{
	case TokenType::ADD:
		superVisitor->asmGen->AppendBinary(ASMInstr::ADD);
		if (hasFloat) returnType = TypeTable::TYPE_FLOAT;
		break;

	case TokenType::SUB:
		superVisitor->asmGen->AppendBinary(ASMInstr::SUB);
		if (hasFloat) returnType = TypeTable::TYPE_FLOAT;
		break;

	case TokenType::MULT:
		superVisitor->asmGen->AppendBinary(ASMInstr::IMUL);
		if (hasFloat) returnType = TypeTable::TYPE_FLOAT;
		break;

	case TokenType::DIV:
		//asmGen->PopValue(ASMReg::EAX); // Extract numerator to EAX
		//asmGen->AppendUnary(ASMInstr::IDIV); // Let method extract denominator

		superVisitor->asmGen->AppendLine("POP eax");
		superVisitor->asmGen->AppendLine("MOV edx, 0");
		superVisitor->asmGen->AppendLine("POP ebx");
		superVisitor->asmGen->AppendLine("IDIV ebx");
		superVisitor->asmGen->AppendLine("PUSH eax");

		if (hasFloat) returnType = TypeTable::TYPE_FLOAT;
		break;

	case TokenType::MOD:
		if (hasFloat) ThrowCompileError("Illegal operator for Floats");
		AppendModulo();
		break;

	case TokenType::AND:
		if (hasFloat) ThrowCompileError("Illegal operator for Floats");
		AppendAnd(expr);
		break;

	case TokenType::OR:
		if (hasFloat) ThrowCompileError("Illegal operator for Floats");
		AppendOr(expr);
		break;

	case TokenType::POW:
		superVisitor->asmGen->PopValue(ASMReg::EBX);
		superVisitor->asmGen->AppendLine("MOV eax, 1");
		superVisitor->asmGen->AppendSpace();
		superVisitor->asmGen->EnterLoop();
		superVisitor->asmGen->AppendLine("IMUL eax, ebx");
		superVisitor->asmGen->ExitLoop();
		superVisitor->asmGen->AppendLine("PUSH eax");
		break;

	case TokenType::EQEQ:
	case TokenType::NEQ:
	case TokenType::GRTR:
	case TokenType::GEQ:
	case TokenType::LESS:
	case TokenType::LEQ:
		AppendCondition(expr->oper->type);
		returnType = TypeTable::TYPE_BOOL;
		break;
	}

	superVisitor->asmGen->AppendSpace();
}

std::string GetConditionInstr(TokenType cond)
{
	switch (cond)
	{
	case TokenType::EQEQ:
		return "JE";
	case TokenType::NEQ:
		return "JNE";
	case TokenType::GRTR:
		return "JG";
	case TokenType::GEQ:
		return "JGE";
	case TokenType::LESS:
		return "JL";
	case TokenType::LEQ:
		return "JLE";
	}

	return "Invalid condition.";
}

void ValueVisitor::AppendCondition(TokenType cond)
{
	std::string instr = GetConditionInstr(cond);
	std::string caseTrueLabel = superVisitor->asmGen->GenerateLabel();
	std::string condExitLabel = superVisitor->asmGen->GenerateLabel();

	superVisitor->asmGen->AppendLine("POP eax");
	superVisitor->asmGen->AppendLine("POP ebx");
	superVisitor->asmGen->AppendLine("cmp eax, ebx");
	superVisitor->asmGen->AppendLine(instr + " " + caseTrueLabel);
	superVisitor->asmGen->AppendLine("PUSH 0");
	superVisitor->asmGen->AppendLine("JMP " + condExitLabel);
	superVisitor->asmGen->AppendLine(caseTrueLabel + ": PUSH 1");
	superVisitor->asmGen->AppendLine(condExitLabel + ":");
}

void ValueVisitor::Visit(const GroupExpr *expr)
{
	superVisitor->asmGen->AppendComment("Evaluate Group");
	expr->value->Accept(this);
}

void ValueVisitor::Visit(const TernExpr *expr)
{
	std::string caseFalseLabel = superVisitor->asmGen->GenerateLabel(); // Incase cond is false
	std::string condExitLabel = superVisitor->asmGen->GenerateLabel(); // End of entire cond expression

	expr->cond->Accept(this); // Pushes condition result into stack

	superVisitor->asmGen->AppendLine("POP eax ;; Save condition result");
	superVisitor->asmGen->AppendLine("CMP eax, 0");
	superVisitor->asmGen->AppendLine("JZ " + caseFalseLabel + " ;; If conditin is false, jump to false label");
	superVisitor->asmGen->AppendSpace();
	expr->caseTrue->Accept(this);
	const Type *trueType = returnType;
	superVisitor->asmGen->AppendLine("JMP " + condExitLabel + " ;; Skip false condition");
	superVisitor->asmGen->AppendSpace();
	superVisitor->asmGen->AppendLine(caseFalseLabel + ":");
	expr->caseFalse->Accept(this);
	const Type *falseType = returnType;
	superVisitor->asmGen->AppendLine(condExitLabel + ":");
	superVisitor->asmGen->AppendSpace();

	if (trueType != falseType)
	{
		ThrowCompileError("Ternary expression results must have same type");
	}
}

void ValueVisitor::Visit(const CondExpr *expr)
{
	expr->cond->Accept(this);
	returnType = TypeTable::TYPE_BOOL;
}