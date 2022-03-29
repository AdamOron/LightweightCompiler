#include "../compiler/Compiler.h"

StatementVisitor::StatementVisitor(StatementVisitor *superVisitor) :
	ChildVisitor(superVisitor),
	asmGen(ASMGenerator::GetInstance()),
	varTable(new VarTable()),
	valueVisitor(new ValueVisitor(this))
{
}

StatementVisitor::StatementVisitor() :
	StatementVisitor(NULL)
{
}

Var *StatementVisitor::GetVar(const Token *id) const
{
	Var *var = varTable->Get(id);

	if (var != NULL)
	{
		return var;
	}

	if (superVisitor != NULL)
	{
		var = superVisitor->GetVar(id);
	}

	return var;
}

void StatementVisitor::Visit(const LitExpr *expr)
{
	/* Let ValueVisitor evaluate */
	expr->Accept(valueVisitor);
}

void StatementVisitor::Visit(const UnaryExpr *expr)
{
	/* Let ValueVisitor evaluate */
	expr->Accept(valueVisitor);
}

void StatementVisitor::Visit(const BinaryExpr *expr)
{
	/* Let ValueVisitor evaluate */
	expr->Accept(valueVisitor);
}

void StatementVisitor::Visit(const GroupExpr *expr)
{
	/* Let ValueVisitor evaluate */
	expr->Accept(valueVisitor);
}

void StatementVisitor::Visit(const TernExpr *expr)
{
	/* Let ValueVisitor evaluate */
	expr->Accept(valueVisitor);
}

void StatementVisitor::Visit(const CondExpr *expr)
{
	/* Let ValueVisitor evaluate */
	expr->Accept(valueVisitor);
}

void StatementVisitor::Visit(const AccessibleExpr *expr)
{
	/* Let ValueVisitor evaluate */
	expr->Accept(valueVisitor);
}

void StatementVisitor::Visit(const PrintExpr *expr)
{
	/* Evaluate & save print value */
	expr->value->Accept(this);

	/* Value is stored in stack, just call print function */
	asmGen->AppendLine("CALL print_number");
}

void StatementVisitor::Visit(const AssignExpr *expr)
{
	/* Extract variable ID from AssignExpr */
	Token *id = expr->var->id;
	/* Get variable that matches extracted ID. If this is null, we need to initialize a new variable */
	Var *var = GetVar(id);

	if (var == NULL)
	{
		ThrowCompileError(id->literal + " is undefined.");
	}

	/* Evaluating the variable's value. This will lead to ValueVisitor evaluating the value. */
	expr->value->Accept(this);
	asmGen->AppendComment("Evaluated variable value, pushed to stack");

	/* Get in advance the pointer for the variable's value */
	// we need to add 4 to this or it doesn't work?
	std::string valPointer = "DWORD [ebp-" + std::to_string(var->memOffset) + "]";

	switch (expr->assignOper->type)
	{
	case TokenType::EQ:
		asmGen->AppendLine("POP " + valPointer);
		break;

	case TokenType::EQ_ADD:
		asmGen->AppendLine("POP eax");
		asmGen->AppendLine("ADD " + valPointer + ", eax");
		break;

	case TokenType::EQ_SUB:
		asmGen->AppendLine("POP eax");
		asmGen->AppendLine("SUB " + valPointer + ", eax");
		break;

	case TokenType::EQ_MULT:
		expr->value->Accept(this);
		asmGen->AppendLine("PUSH " + valPointer);
		asmGen->AppendBinary(ASMInstr::IMUL);
		asmGen->AppendLine("POP " + valPointer);
		break;

	case TokenType::EQ_DIV:
		expr->value->Accept(this);
		asmGen->AppendLine("PUSH " + valPointer);
		asmGen->AppendBinary(ASMInstr::IDIV);
		asmGen->AppendLine("POP " + valPointer);
		break;
	}

	asmGen->AppendSpace();
}

void StatementVisitor::Visit(const InitExpr *expr)
{
	/* Extract variable ID from AssignExpr */
	Token *id = expr->id;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO: BUG HERE. WE MUST PUSH VALUE ONLY AFTER MOVING ESP.
	// HOW CAN WE KNOW THE TYPE BEFORE EVALUATING IT?
	// 1 OPTION IS TO USE TYPES, BUT THAT'S TOO MUCH WORK AND NOT THE POINT OF OUR LANG
	// 2 OPTION IS TO POP EVALUATED INTO REGISTER, THEN MOV ESP, THEN PUSH AGAIN
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	/* Evaluating the variable's value. This will lead to ValueVisitor evaluating the value. */
	expr->value->Accept(this);
	asmGen->AppendComment("Evaluated variable value, saving to eax");
	asmGen->AppendLine("POP eax");

	/* Save the evaluated value's Type, as it is the new variable's Type*/
	const Type *type = valueVisitor->GetType();

	// Print type, delete after debugging
	//std::cout << expr->var->id->literal << ' ' << type->size << '\n';

	/* Add variable to variable table.
	* We use TYPE_INT at this stage because we haven't implemented any other size handling (only 4bytes supported).
	* Ideally & eventually, we will handle smaller/larger byte sizes properly, in order to save space.
	*/
	Var *var = varTable->Add(expr->id, TypeTable::TYPE_INT);

	if (var == NULL)
	{
		ThrowCompileError(var->id->literal + " is already defined within this scope.");
	}

	/* Allocate stack memory for the new variable */
	asmGen->AppendLine("SUB esp, " + std::to_string(var->type->size)); // Allocate memory for variable

	asmGen->AppendLine("PUSH eax");
	asmGen->AppendComment("Pushing eax after allocating storage");

	/* Get in advance the pointer for the variable's value */
	// we need to add 4 to this or it doesn't work?
	std::string valPointer = "DWORD [ebp-" + std::to_string(var->memOffset) + "]";

	asmGen->AppendLine("POP " + valPointer);
	asmGen->AppendSpace();
}

void StatementVisitor::VisitCondition(const IfExpr *expr, std::string &exitLabel)
{
	std::string falseLabel = asmGen->GenerateLabel(); // Incase cond is false

	expr->cond->Accept(this); // Pushes condition result into stack

	asmGen->AppendLine("POP eax ;; Save condition result");
	asmGen->AppendLine("CMP eax, 0");
	asmGen->AppendLine("JZ " + falseLabel + " ;; If conditin is false, jump to false label");
	asmGen->AppendSpace();

	StatementVisitor *ifVisitor = new StatementVisitor(this);
	expr->block->Accept(ifVisitor);

	asmGen->AppendLine("JMP " + exitLabel);
	asmGen->AppendLine(falseLabel + ":");
	if (expr->elif != NULL) VisitCondition(expr->elif, exitLabel);
}

void StatementVisitor::Visit(const IfExpr *expr)
{
	std::string exitLabel = asmGen->GenerateLabel();

	VisitCondition(expr, exitLabel);
	asmGen->AppendLine(exitLabel + ":");
}

void StatementVisitor::Visit(const ElseExpr *expr)
{
	std::string exitLabel = asmGen->GenerateLabel();

	VisitCondition(expr->ifExpr, exitLabel);
	Visit(expr->ifExpr);
	asmGen->AppendLine(exitLabel + ":");
}

void StatementVisitor::Visit(const ControlFlowExpr *expr)
{
	ThrowCompileError("Control flow statement cannot be used outside of controllable expression.");
}

void StatementVisitor::Visit(const WhileExpr *expr)
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

void StatementVisitor::Visit(const ForExpr *expr)
{
	std::string loopStartLabel = asmGen->GenerateLabel();
	std::string loopExitLabel = asmGen->GenerateLabel();

	expr->assign->Accept(this);

	asmGen->AppendLine(loopStartLabel + ":");

	expr->cond->Accept(this);
	asmGen->AppendLine("POP eax");
	asmGen->AppendLine("CMP eax, 0");
	asmGen->AppendLine("JZ " + loopExitLabel);

	std::string loopIncrLabel = asmGen->GenerateLabel();

	ControllableVisitor whileVisitor(this, loopIncrLabel, loopExitLabel);
	expr->block->Accept(&whileVisitor);

	asmGen->AppendLine(loopIncrLabel + ":");
	expr->incr->Accept(this);
	asmGen->AppendLine("JMP " + loopStartLabel);

	asmGen->AppendLine(loopExitLabel + ":");
}

void StatementVisitor::Visit(const FuncExpr *expr)
{
}

void StatementVisitor::Visit(const ExprGroup *block)
{
	for (auto expr : block->exprs)
		expr->Accept(this);
}