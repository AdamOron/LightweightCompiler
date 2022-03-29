#pragma once
#include "StatementVisitor.h"

/**
* The ControllableVisitor inherits the StatementVisitor  and uses all of its Visit() functions, with the exception of the
* ControlFlowExpr which is only implemented in this Visitor.
* This type of Visitor is used for whenever we encounter a controllable expression, and it only handles its scope, for example:
* handling the body of a for-loop, handling the 
*/
class ControllableVisitor : public StatementVisitor
{
private:
	/* Each Controllable expression has an enter & exit label, so they can be controlled with continue/break statements */
	const std::string labelEnter, labelExit;

public:
	/**
	* A ControllableVisitor is constructed with its calling BaseVisitor & its Controllable expression's enter & exit labels.
	*/
	ControllableVisitor(StatementVisitor *superVisitor, const std::string labelEnter, const std::string labelExit);
	/**
	* Get Variable from VarTable.
	* This overrides StatementVisitor's GetVar() to allow getting variables from outer scopes.
	*/
	Var *GetVar(const Token *id) const override;
	/**
	* The only implemented Visit() function is for ControlFlowExpr.
	* This function continues/exits the Controllable expression.
	*/
	void Visit(const ControlFlowExpr *expr);
};