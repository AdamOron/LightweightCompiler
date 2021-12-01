#pragma once
#include "Expr.h"
#include "VarTable.h"
#include "ASMGenerator.h"
#include <string>
#include <stack>
#include <vector>

class BaseVisitor : public IVisitor
{
private:
	void HandleNot();
	void HandleAnd(const BinaryExpr* expr);
	void HandleOr(const BinaryExpr* expr);
	void HandleModulo();
	void AppendCondition(TokenType cond);
	void VisitCondition(const IfExpr* expr, std::string& exitLabel);

public:
	ASMGenerator *asmGen;
	VarTable *varTable;

	BaseVisitor();
	void Visit(const LitExpr *expr);
	void Visit(const UnaryExpr *expr);
	void Visit(const BinaryExpr *expr);
	void Visit(const GroupExpr *expr);
	void Visit(const TernExpr *expr);
	void Visit(const CondExpr *expr);
	void Visit(const AccessibleExpr *expr);
	void Visit(const ArrayExpr *expr) {}
	void Visit(const PrintExpr *expr);
	void Visit(const AssignExpr *expr);
	void Visit(const IfExpr *expr);
	void Visit(const ElseExpr *expr);
	void Visit(const ControlFlowExpr *expr);
	void Visit(const WhileExpr *expr);
	void Visit(const ForExpr *expr);
	void Visit(const ExprBlock *block);
};

class ControllableVisitor : public BaseVisitor
{
private:
	const IVisitor *superVisitor;
	const std::string labelEnter, labelExit;

public:
	ControllableVisitor(const BaseVisitor *superVisitor, const std::string labelEnter, const std::string labelExit);
	void Visit(const ControlFlowExpr *expr);
};

std::string Compile(const ExprBlock *block);
