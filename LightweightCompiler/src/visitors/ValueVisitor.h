#pragma once
#include "IVisitor.h"

class StatementVisitor;

/**
* This is used for evaluating any value expr. This is perhaps redundant (it can simply implemented within the StatementVisitor), and also the empty
* functions for unimplemented Visits (as can be seen below) are quite ugly, but this is currently a better solution and it allows separation for statement
* exprs and value exprs.
*/
class ValueVisitor : public ChildVisitor<StatementVisitor>
{
private:
	/**
	* A ValueVisitor keeps track of the evaluated value's Type.
	* This is a replacement for a generic return-type visitor pattern.
	*/
	const Type *returnType;

	/* Handles boolean NOT on value that's pushed to stack. */
	void AppendNot();
	/* Handles boolean AND on given BinaryExpr */
	void AppendAnd(const BinaryExpr *expr);
	/* Handles boolean OR on given BinaryExpr */
	void AppendOr(const BinaryExpr *expr);
	/* Handles MODULO operation on two ints pushed to stack */
	void AppendModulo();
	/* Appends ASM condition matching given condition type (==, !=, >, etc) */
	void AppendCondition(TokenType cond);

public:
	/**
	* Construct new ValueVisitor with given BaseVisitor as its caller.
	*/
	ValueVisitor(StatementVisitor *superVisitor);
	/**
	* Get the evaluated Type.
	*/
	const Type *GetType();

	/* ValueVisitor handles all value expressions */
	void Visit(const LitExpr *expr);
	void Visit(const UnaryExpr *expr);
	void Visit(const BinaryExpr *expr);
	void Visit(const GroupExpr *expr);
	void Visit(const TernExpr *expr);
	void Visit(const CondExpr *expr);
	void Visit(const AccessibleExpr *expr);

	/* Will not encounter/handle any of these expressions */
	void Visit(const ArrayExpr *expr) {}
	void Visit(const PrintExpr *expr) {}
	void Visit(const AssignExpr *expr) {}
	void Visit(const InitExpr *expr) {}
	void Visit(const IfExpr *expr) {}
	void Visit(const ElseExpr *expr) {}
	void Visit(const ControlFlowExpr *expr) {}
	void Visit(const WhileExpr *expr) {}
	void Visit(const ForExpr *expr) {}
	void Visit(const ExprGroup *block) {}
};