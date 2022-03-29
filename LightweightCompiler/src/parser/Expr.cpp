#include "Expr.h"

void LitExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void UnaryExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void BinaryExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void GroupExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void TernExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void CondExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void AccessibleExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void ArrayExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void PrintExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void InitExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void AssignExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void IfExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void ElseExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void ControlFlowExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void WhileExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void ForExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void ExprGroup::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}

void FuncExpr::Accept(IVisitor *visitor) const
{
	return visitor->Visit(this);
}
