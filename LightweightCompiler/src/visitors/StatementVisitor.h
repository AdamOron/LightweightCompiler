#pragma once
#include "IVisitor.h"

class ValueVisitor;

class StatementVisitor : public ChildVisitor<StatementVisitor>
{
protected:
	/* Each StatementVisitor has a varTable that keeps track of all of its variables */
	VarTable *varTable;
	TypeTable *typeTable;
	/**
	* Each StatementVisitor has a ValueVisitor that's used for evaluating values & getting their Type.
	*/
	ValueVisitor *valueVisitor;

	/**
	* @param expr the IfExpr to handle.
	* @param exitLabel the label which indicates the end of the expr. If the condition doesn't hold, we go there.
	*/
	void VisitCondition(const IfExpr *expr, std::string &exitLabel);

public:
	/* Each StatementVisitor has an ASMGenerator that it uses to create the ASM file. Feels unsafe to have this public, but will do for now */
	ASMGenerator *asmGen;

	/* StatementVisitor that has a super Visitor */
	StatementVisitor(StatementVisitor *visitor);
	/* StatementVisitor that has no super Visitor (the first StatementVisitor) */
	StatementVisitor();

	/**
	* Wrapper function for getting a variable.
	* This is virtual to allow inheriting classes to use a different implementation of getting variables,
	* For example: inner scopes can use variables from outer scopes (but not the opposite), etc.
	*/
	virtual Var *GetVar(const Token *id) const;

	void Visit(const LitExpr *expr) override;
	void Visit(const UnaryExpr *expr) override;
	void Visit(const BinaryExpr *expr) override;
	void Visit(const GroupExpr *expr) override;
	void Visit(const TernExpr *expr) override;
	void Visit(const CondExpr *expr) override;
	void Visit(const AccessibleExpr *expr);
	void Visit(const ArrayExpr *expr) override {} // Unimplemented
	void Visit(const PrintExpr *expr) override;
	void Visit(const AssignExpr *expr) override;
	void Visit(const InitExpr *expr) override;
	void Visit(const IfExpr *expr) override;
	void Visit(const ElseExpr *expr) override;
	void Visit(const ControlFlowExpr *expr) override;
	void Visit(const WhileExpr *expr) override;
	void Visit(const ForExpr *expr) override;
	void Visit(const FuncExpr *expr) override;
	void Visit(const ExprGroup *block) override;
};