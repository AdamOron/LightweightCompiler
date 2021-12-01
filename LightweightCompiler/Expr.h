#pragma once
#include <algorithm>
#include "Lexer.h"

class IVisitor;

/**
* Abstract class that represents a generic expression.
* Includes virtual Repr method that allows printing each expression (used for debugging expressions).
*/
class Expr
{
public:
	virtual std::ostream &Repr(std::ostream &stream) const = 0;

	virtual void Accept(IVisitor *visitor) const = 0;
};

inline std::ostream &operator<<(std::ostream &ostrm, const Expr &expr)
{
	return expr.Repr(ostrm);
}

class ExprBlock : public Expr
{
public:
	std::vector<const Expr *> *exprs;

	ExprBlock()
	{
		this->exprs = new std::vector<const Expr *>();
	}

	void Add(const Expr *expr)
	{
		this->exprs->push_back(expr);
	}

	void Accept(IVisitor *visitor) const override;

	std::ostream &Repr(std::ostream &stream) const override
	{
		for (auto iter = exprs->begin(); iter != exprs->end(); iter++)
		{
			(*iter)->Repr(stream);
			stream << '\n';
		}

		return stream;
	}
};

/**
* Implementation for a literal expression (INT/FLOAT/BOOL/CHAR/STRING).
*/
class LitExpr : public Expr
{
public:
	Token *value;

	LitExpr(Token *value) :
		value(value)
	{
	}

	std::ostream &Repr(std::ostream &stream) const override
	{
		return stream << *value;
	}

	void Accept(IVisitor *visitor) const override;
};

/**
* Implementation for a unary expression.
*/
class UnaryExpr : public Expr
{
public:
	Token *oper;
	Expr *value;

	UnaryExpr(Token *oper, Expr *value) :
		oper(oper),
		value(value)
	{
	}

	std::ostream &Repr(std::ostream &stream) const override
	{
		stream << '(';
		stream << *oper << ' ';
		value->Repr(stream);
		stream << ')';
		return stream;
	}

	void Accept(IVisitor *visitor) const override;
};

class BinaryExpr : public Expr
{
public:
	Expr *left, *right;
	Token *oper;

	BinaryExpr(Expr *left, Expr *right, Token *oper) :
		left(left),
		right(right),
		oper(oper)
	{
	}

	std::ostream &Repr(std::ostream &stream) const override
	{
		stream << '(';
		left->Repr(stream);
		stream << ' ' << *oper << ' ';
		right->Repr(stream);
		stream << ')';
		return stream;
	}

	void Accept(IVisitor *visitor) const override;
};

class GroupExpr : public Expr
{
public:
	Expr *value;

	GroupExpr(Expr *value) :
		value(value)
	{
	}

	std::ostream &Repr(std::ostream &stream) const override
	{
		value->Repr(stream);
		return stream;
	}

	void Accept(IVisitor *visitor) const override;
};

class TernExpr : public Expr
{
public:
	Expr *cond, *caseTrue, *caseFalse;

	TernExpr(Expr *cond, Expr *caseTrue, Expr *caseFalse) :
		cond(cond),
		caseTrue(caseTrue),
		caseFalse(caseFalse)
	{
	}

	std::ostream &Repr(std::ostream &stream) const override
	{
		stream << '(';
		cond->Repr(stream);
		stream << " ? ";
		caseTrue->Repr(stream);
		stream << " : ";
		caseFalse->Repr(stream);
		stream << ')';
		return stream;
	}

	void Accept(IVisitor *visitor) const override;
};

/**
* Implementation for a conditional expression.
* This merely wraps a normal expression. Evaluation of a CondExpr should automatically do "CMP <cond>, 0".
*/
class CondExpr : public Expr
{
public:
	Expr *cond;

	CondExpr(Expr *cond) :
		cond(cond)
	{
	}

	std::ostream &Repr(std::ostream &stream) const override
	{
		return cond->Repr(stream);
	}

	void Accept(IVisitor *visitor) const override;
};

/**
* Implementation for an accessible expression (VAR/ARRAY).
*/
class AccessibleExpr : public Expr
{
public:
	Token *id;

	AccessibleExpr(Token *id) :
		id(id)
	{
	}

	std::ostream &Repr(std::ostream &stream) const override
	{
		stream << *id;
		return stream;
	}

	void Accept(IVisitor *visitor) const override;
};

/**
* Implementation for an array initializer expression (e.g. [1, 2, 3]).
*/
class ArrayExpr : public Expr
{
public:

};

/**
* Implementation for a print call.
* This is a placeholder as we haven't implemented proper methods yet.
*/
class PrintExpr : public Expr
{
public:
	Expr *value;

	PrintExpr(Expr *value) :
		value(value)
	{
	}

	std::ostream &Repr(std::ostream &stream) const override
	{
		stream << "print (";
		value->Repr(stream);
		stream << ')';
		return stream;
	}

	void Accept(IVisitor *visitor) const override;
};

/**
* Implementation for an assign expression.
*/
class AssignExpr : public Expr
{
public:
	AccessibleExpr *var;
	Token *assignOper;
	Expr *value;

	AssignExpr(AccessibleExpr *var, Token *assignOper, Expr *value) :
		var(var),
		assignOper(assignOper),
		value(value)
	{
	}

	std::ostream &Repr(std::ostream &stream) const override
	{
		stream << '(';
		var->Repr(stream);
		stream << ' ' << *assignOper << ' ';
		value->Repr(stream);
		stream << ')';
		return stream;
	}

	void Accept(IVisitor *visitor) const override;
};

class IfExpr : public Expr
{
public:
	CondExpr *cond;
	ExprBlock *block;
	IfExpr *elif; // Chain elif expressions

	IfExpr(CondExpr *cond, ExprBlock *block, IfExpr *elif) :
		cond(cond),
		block(block),
		elif(elif)
	{
	}

	IfExpr(CondExpr *cond, ExprBlock *block) :
		IfExpr(cond, block, NULL)
	{
	}

	std::ostream &Repr(std::ostream &stream) const override
	{
		stream << "if (";
		cond->Repr(stream);
		stream << ")\n{\n";
		stream << *block;
		stream << "\n}";

		if (elif != NULL)
		{
			stream << "\nel";
			elif->Repr(stream);
		}

		return stream;
	}

	void Accept(IVisitor *visitor) const override;
};

class ElseExpr : public Expr
{
public:
	IfExpr *ifExpr;
	ExprBlock *block;

	ElseExpr(IfExpr *ifExpr, ExprBlock *block) :
		ifExpr(ifExpr),
		block(block)
	{
	}

	std::ostream &Repr(std::ostream &stream) const override
	{
		ifExpr->Repr(stream);
		stream << "\nelse\n{\n";
		stream << *block;
		stream << "\n}";
		return stream;
	}

	void Accept(IVisitor *visitor) const override;
};

class ControlFlowExpr : public Expr
{
public:
	Token *stmt;

	ControlFlowExpr(Token *stmt) :
		stmt(stmt)
	{
	}

	std::ostream &Repr(std::ostream &stream) const override
	{
		stream << *stmt;
		return stream;
	}

	void Accept(IVisitor *visitor) const override;
};

class WhileExpr : public Expr
{
public:
	CondExpr *cond;
	ExprBlock *block;

	WhileExpr(CondExpr *cond, ExprBlock *block) :
		cond(cond),
		block(block)
	{
	}

	void Accept(IVisitor *visitor) const override;

	std::ostream &Repr(std::ostream &stream) const override
	{
		stream << "while (";
		cond->Repr(stream);
		stream << ")\n{\n";
		stream << *block;
		stream << "\n}";
		return stream;
	}
};

class ForExpr : public Expr
{
public:
	Expr *assign, *incr;
	CondExpr *cond;
	ExprBlock *block;

	ForExpr(Expr *assign, CondExpr *cond, Expr *incr, ExprBlock *block) :
		assign(assign),
		cond(cond),
		incr(incr),
		block(block)
	{
	}

	void Accept(IVisitor *visitor) const override;

	std::ostream &Repr(std::ostream &stream) const override
	{
		stream << "for (";
		assign->Repr(stream);
		stream << "; ";
		cond->Repr(stream);
		stream << "; ";
		incr->Repr(stream);
		stream << ")\n{\n";
		stream << *block;
		stream << "\n}";
		return stream;
	}
};

class IVisitor
{
public:
	virtual ~IVisitor() {}
	virtual void Visit(const ExprBlock *expr) = 0;
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
};
