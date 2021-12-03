#include "Compiler.h"
#include "Parser.h"

void ThrowError(std::string error)
{
	std::cout << "COMPILE ERROR: " << error;
	exit(1);
}

ControllableVisitor::ControllableVisitor(const BaseVisitor *superVisitor, const std::string labelEnter, const std::string labelExit) :
	BaseVisitor(),
	superVisitor(superVisitor),
	labelEnter(labelEnter),
	labelExit(labelExit)
{
}

Var *ControllableVisitor::GetVar(const Token *id) const
{
	Var *current = varTable->Get(id);

	if (current == NULL)
	{
		return superVisitor->GetVar(id);
	}

	return current;
}

void ControllableVisitor::Visit(const ControlFlowExpr *expr)
{
	switch (expr->stmt->type)
	{
	case TokenType::BREAK:
		asmGen->AppendLine("JMP " + labelExit);
		break;

	case TokenType::CONTINUE:
		asmGen->AppendLine("JMP " + labelEnter + " ; continue");
		break;
	}
}

BaseVisitor::BaseVisitor()
{
	this->asmGen = ASMGenerator::GetInstance();
	this->varTable = new VarTable();
}

Var *BaseVisitor::GetVar(const Token *id) const
{
	return varTable->Get(id);
}

void BaseVisitor::Visit(const LitExpr *expr)
{
	if (expr->value->type == TokenType::BOOL)
	{
		asmGen->PushValue(expr->value->literal == Token::FALSE_LIT ? "0" : "1");
		return;
	}

	if (expr->value->type == TokenType::INT)
	{
		asmGen->PushValue(expr->value->literal);
		return;
	}

	ThrowError("Invalid literal type");
}

void BaseVisitor::Visit(const AccessibleExpr *expr)
{
	Token *id = expr->id;
	Var *var = GetVar(id);

	if (var == NULL)
	{
		ThrowError("Invalid accessor name " + id->literal);
	}

	asmGen->PushValue("[ebp-" + std::to_string(var->memOffset) + "]");
}

void BaseVisitor::HandleNot()
{
	std::string isFalse = asmGen->GenerateLabel();
	std::string exit = asmGen->GenerateLabel();

	asmGen->AppendLine("POP edx");

	asmGen->AppendLine("CMP edx, 0");
	asmGen->AppendLine("JZ " + isFalse);

	asmGen->AppendLine("PUSH 0");
	asmGen->AppendLine("JMP " + exit);
	asmGen->AppendLine(isFalse + ": PUSH 1");
	asmGen->AppendLine(exit + ":");
}

void BaseVisitor::Visit(const UnaryExpr *expr)
{
	expr->value->Accept(this);

	switch (expr->oper->type)
	{
	case TokenType::SUB:
		asmGen->AppendUnary(ASMInstr::NEG);
		break;

	case TokenType::NOT:
		HandleNot();
		break;
	}

	asmGen->AppendSpace();
}

void BaseVisitor::HandleAnd(const BinaryExpr *expr)
{
	std::string hasZero = asmGen->GenerateLabel();
	std::string exit = asmGen->GenerateLabel();

	asmGen->AppendLine("POP eax");
	asmGen->AppendLine("POP ebx");

	asmGen->AppendLine("CMP eax, 0");
	asmGen->AppendLine("JZ " + hasZero);
	asmGen->AppendLine("CMP ebx, 0");
	asmGen->AppendLine("JZ " + hasZero);

	asmGen->AppendLine("PUSH 1");
	asmGen->AppendLine("JMP " + exit);
	asmGen->AppendLine(hasZero + ": PUSH 0");
	asmGen->AppendLine(exit + ":");
}

void BaseVisitor::HandleOr(const BinaryExpr *expr)
{
	std::string hasOne = asmGen->GenerateLabel();
	std::string exit = asmGen->GenerateLabel();

	asmGen->AppendLine("POP eax");
	asmGen->AppendLine("POP ebx");

	asmGen->AppendLine("CMP eax, 0");
	asmGen->AppendLine("JNZ " + hasOne);
	asmGen->AppendLine("CMP ebx, 0");
	asmGen->AppendLine("JNZ " + hasOne);

	asmGen->AppendLine("PUSH 0");
	asmGen->AppendLine("JMP " + exit);
	asmGen->AppendLine(hasOne + ": PUSH 1");
	asmGen->AppendLine(exit + ":");
}

void BaseVisitor::HandleModulo()
{
	asmGen->AppendLine("POP eax");
	asmGen->AppendLine("MOV edx, 0");
	asmGen->AppendLine("POP ebx");
	asmGen->AppendLine("IDIV ebx");
	asmGen->AppendLine("PUSH edx");
}

void BaseVisitor::Visit(const BinaryExpr *expr)
{
	/* We push values from right to left due to ASM's LIFO stack */
	expr->right->Accept(this); // Push evaluated right expression onto the ASM stack
	expr->left->Accept(this); // Push evaluated left expression onto the ASM stack

	switch (expr->oper->type)
	{
	case TokenType::ADD:
		asmGen->AppendBinary(ASMInstr::ADD);
		break;

	case TokenType::SUB:
		asmGen->AppendBinary(ASMInstr::SUB);
		break;

	case TokenType::MULT:
		asmGen->AppendBinary(ASMInstr::IMUL);
		break;

	case TokenType::DIV:
		//asmGen->PopValue(ASMReg::EAX); // Extract numerator to EAX
		//asmGen->AppendUnary(ASMInstr::IDIV); // Let method extract denominator

		asmGen->AppendLine("POP eax");
		asmGen->AppendLine("MOV edx, 0");
		asmGen->AppendLine("POP ebx");
		asmGen->AppendLine("IDIV ebx");
		asmGen->AppendLine("PUSH eax");

		break;

	case TokenType::MOD:
		HandleModulo();
		break;

	case TokenType::AND:
		HandleAnd(expr);
		break;

	case TokenType::OR:
		HandleOr(expr);
		break;

	case TokenType::POW:
		asmGen->PopValue(ASMReg::EBX);
		asmGen->AppendLine("MOV eax, 1");
		asmGen->AppendSpace();
		asmGen->EnterLoop();
		asmGen->AppendLine("IMUL eax, ebx");
		asmGen->ExitLoop();
		asmGen->AppendLine("PUSH eax");
		break;

	case TokenType::EQEQ:
	case TokenType::NEQ:
	case TokenType::GRTR:
	case TokenType::GEQ:
	case TokenType::LESS:
	case TokenType::LEQ:
		AppendCondition(expr->oper->type);
		break;
	}

	asmGen->AppendSpace();
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

void BaseVisitor::AppendCondition(TokenType cond)
{
	std::string instr = GetConditionInstr(cond);
	std::string caseTrueLabel = asmGen->GenerateLabel();
	std::string condExitLabel = asmGen->GenerateLabel();

	asmGen->AppendLine("POP eax");
	asmGen->AppendLine("POP ebx");
	asmGen->AppendLine("cmp eax, ebx");
	asmGen->AppendLine(instr + " " + caseTrueLabel);
	asmGen->AppendLine("PUSH 0");
	asmGen->AppendLine("JMP " + condExitLabel);
	asmGen->AppendLine(caseTrueLabel + ": PUSH 1");
	asmGen->AppendLine(condExitLabel + ":");
}

void BaseVisitor::Visit(const GroupExpr *expr)
{
	asmGen->AppendComment("Evaluate Group");
	expr->value->Accept(this);
}

void BaseVisitor::Visit(const TernExpr *expr)
{
	std::string caseFalseLabel = asmGen->GenerateLabel(); // Incase cond is false
	std::string condExitLabel = asmGen->GenerateLabel(); // End of entire cond expression

	expr->cond->Accept(this); // Pushes condition result into stack

	asmGen->AppendLine("POP eax ;; Save condition result");
	asmGen->AppendLine("CMP eax, 0");
	asmGen->AppendLine("JZ " + caseFalseLabel + " ;; If conditin is false, jump to false label");
	asmGen->AppendSpace();
	expr->caseTrue->Accept(this);
	asmGen->AppendLine("JMP " + condExitLabel + " ;; Skip false condition");
	asmGen->AppendSpace();
	asmGen->AppendLine(caseFalseLabel + ":");
	expr->caseFalse->Accept(this);
	asmGen->AppendLine(condExitLabel + ":");
	asmGen->AppendSpace();
}

void BaseVisitor::Visit(const CondExpr* expr)
{
	return expr->cond->Accept(this);
}

void BaseVisitor::Visit(const PrintExpr *expr)
{
	expr->value->Accept(this);

	asmGen->AppendLine("POP eax");
	asmGen->AppendLine("print sstr$(eax), 13, 10");
}

void BaseVisitor::Visit(const AssignExpr *expr)
{
	/* Extract variable ID from AssignExpr */
	Token *id = expr->var->id;
	/* Get variable that matches extracted ID. If this is null, we need to initialize a new variable */
	Var *var = GetVar(id);

 	/* Check if we need to initialize a new variable */
	bool toAdd = var == NULL;

	expr->value->Accept(this);
	asmGen->AppendSpace();

	if (toAdd)
	{
		var = varTable->Add(expr->var->id);
		asmGen->AppendLine("SUB esp, " + std::to_string(var->type->size)); // Allocate memory for variable
	}

	std::string memAddress = "[ebp-" + std::to_string(var->memOffset) + "]";

	switch (expr->assignOper->type)
	{
	case TokenType::EQ:
		asmGen->AppendLine("POP " + memAddress);
		break;

	case TokenType::EQ_ADD:
		asmGen->AppendLine("POP eax");
		asmGen->AppendLine("ADD " + memAddress + ", eax");
		break;

	case TokenType::EQ_SUB:
		asmGen->AppendLine("POP eax");
		asmGen->AppendLine("SUB " + memAddress + ", eax");
		break;

	case TokenType::EQ_MULT:
		expr->value->Accept(this);
		asmGen->AppendLine("PUSH " + memAddress);
		asmGen->AppendBinary(ASMInstr::IMUL);
		asmGen->AppendLine("POP " + memAddress);
		break;

	case TokenType::EQ_DIV:
		expr->value->Accept(this);
		asmGen->AppendLine("PUSH " + memAddress);
		asmGen->AppendBinary(ASMInstr::IDIV);
		asmGen->AppendLine("POP " + memAddress);
		break;
	}

	asmGen->AppendSpace();
}

void BaseVisitor::VisitCondition(const IfExpr *expr, std::string &exitLabel)
{
	std::string falseLabel = asmGen->GenerateLabel(); // Incase cond is false

	expr->cond->Accept(this); // Pushes condition result into stack

	asmGen->AppendLine("POP eax ;; Save condition result");
	asmGen->AppendLine("CMP eax, 0");
	asmGen->AppendLine("JZ " + falseLabel + " ;; If conditin is false, jump to false label");
	asmGen->AppendSpace();
	Visit(expr->block);
	asmGen->AppendLine("JMP " + exitLabel);
	asmGen->AppendLine(falseLabel + ":");
	if (expr->elif != NULL) VisitCondition(expr->elif, exitLabel);
}

void BaseVisitor::Visit(const IfExpr *expr)
{
	std::string exitLabel = asmGen->GenerateLabel();

	VisitCondition(expr, exitLabel);
	asmGen->AppendLine(exitLabel + ":");
}

void BaseVisitor::Visit(const ElseExpr *expr)
{
	std::string exitLabel = asmGen->GenerateLabel();

	VisitCondition(expr->ifExpr, exitLabel);
	Visit(expr->ifExpr);
	asmGen->AppendLine(exitLabel + ":");
}

void BaseVisitor::Visit(const ControlFlowExpr *expr)
{
	ThrowError("Control flow statement cannot be used outside of controllable expression.");
}

void BaseVisitor::Visit(const WhileExpr *expr)
{
	std::string loopStartLabel = asmGen->GenerateLabel();
	std::string loopExitLabel = asmGen->GenerateLabel();

	asmGen->AppendLine(loopStartLabel + ":");
	expr->cond->Accept(this);
	asmGen->AppendLine("POP eax");
	asmGen->AppendLine("CMP eax, 0");
	asmGen->AppendLine("JZ " + loopExitLabel);

	ControllableVisitor whileVisitor(this, loopStartLabel, loopExitLabel);
	expr->block->Accept(&whileVisitor);

	asmGen->AppendLine("JMP " + loopStartLabel);
	asmGen->AppendLine(loopExitLabel + ":");
}

void BaseVisitor::Visit(const ForExpr *expr)
{
	/*std::string loopStartLabel = asmGen->GenerateLabel();
	int iLoopExit = asmGen->LabelCount();
	std::string loopExitLabel = asmGen->GenerateLabel();

	expr->assign->Accept(this);

	asmGen->AppendLine(loopStartLabel + ":");

	expr->cond->Accept(this);
	asmGen->AppendLine("POP eax");
	asmGen->AppendLine("CMP eax, 0");
	asmGen->AppendLine("JZ " + loopExitLabel);

	int iLoopStart = asmGen->LabelCount();
	std::string loopStart = asmGen->GenerateLabel();
	BlockFlow flow = BlockFlow(iLoopStart, iLoopExit);
	blocks->push(&flow);
	Visit(expr->whileTrue);
	asmGen->AppendLine(loopStart + ":");
	blocks->pop();

	expr->incr->Accept(this);
	asmGen->AppendLine("JMP " + loopStartLabel);

	asmGen->AppendLine(loopExitLabel + ":");*/
}

void BaseVisitor::Visit(const ExprBlock *block)
{
	for (auto iter = block->exprs->begin(); iter != block->exprs->end(); iter++)
	{
		(*iter)->Accept(this);
	}
}

std::string Compile(const ExprBlock *block)
{
	BaseVisitor visitor;

	visitor.asmGen->FilePrologue();
	//visitor.asmGen->EnterMethod();

	visitor.Visit(block);

	//visitor.asmGen->ExitMethod();
	visitor.asmGen->FileEpilogue();

	return visitor.asmGen->code;
}
