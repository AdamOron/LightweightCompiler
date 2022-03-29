 #pragma once

class ExprGroup;
class LitExpr;
class UnaryExpr;
class BinaryExpr;
class GroupExpr;
class TernExpr;
class CondExpr;
class AccessibleExpr;
class ArrayExpr;
class PrintExpr;
class AssignExpr;
class IfExpr;
class ElseExpr;
class ControlFlowExpr;
class WhileExpr;
class ForExpr;
class FuncExpr;

/**
* Defines basic Visitor behavior. Should be extended by any other Visitor type.
*/
class IVisitor
{
public:
	virtual void Visit(const ExprGroup *expr) = 0;
	virtual void Visit(const LitExpr *expr) = 0;
	virtual void Visit(const UnaryExpr *expr) = 0;
	virtual void Visit(const BinaryExpr *expr) = 0;
	virtual void Visit(const GroupExpr *expr) = 0;
	virtual void Visit(const TernExpr *expr) = 0;
	virtual void Visit(const CondExpr *expr) = 0;
	virtual void Visit(const AccessibleExpr *expr) = 0;
	virtual void Visit(const ArrayExpr *expr) = 0;
	virtual void Visit(const PrintExpr *expr) = 0;
	virtual void Visit(const AssignExpr *expr) = 0;
	virtual void Visit(const IfExpr *expr) = 0;
	virtual void Visit(const ElseExpr *expr) = 0;
	virtual void Visit(const ControlFlowExpr *expr) = 0;
	virtual void Visit(const WhileExpr *expr) = 0;
	virtual void Visit(const ForExpr *expr) = 0;
	virtual void Visit(const FuncExpr *expr) {};
};

/**
* Defines a child Visitor behavior, a Visitor that was called by another Visitor.
* The calling Visitor is unknown, so we have a template for it.
*/
template <typename SuperVisitor>
class ChildVisitor : public IVisitor
{
protected:
	/* Every ChildVisitor is called by another Visitor. That Visitor's type is received as a template. */
	SuperVisitor *superVisitor;

public:
	ChildVisitor(SuperVisitor *superVisitor) :
		superVisitor(superVisitor)
	{
	}
};
