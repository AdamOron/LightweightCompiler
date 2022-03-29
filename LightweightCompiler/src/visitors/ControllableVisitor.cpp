#include "../compiler/Compiler.h"

ControllableVisitor::ControllableVisitor(StatementVisitor *superVisitor, const std::string labelEnter, const std::string labelExit) :
	StatementVisitor(superVisitor),
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
		asmGen->AppendLine("JMP " + labelExit + " ; break");
		break;

	case TokenType::CONTINUE:
		asmGen->AppendLine("JMP " + labelEnter + " ; continue");
		break;
	}
}