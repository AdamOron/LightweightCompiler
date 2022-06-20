#pragma once
#include "../tokens/Token.h"
#include "../visitors/IVisitor.h"
#include "../tables/VarTable.h"
#include <algorithm>

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

class ExprGroup : public Expr
{
public:
	std::vector<const Expr *> exprs;

	ExprGroup()
	{
		this->exprs = std::vector<const Expr *>();
	}

	void Add(const Expr *expr)
	{
		this->exprs.push_back(expr);
	}

	void Accept(IVisitor *visitor) const override;

	std::ostream &Repr(std::ostream &stream) const override
	{
		for (auto expr : exprs)
		{
			expr->Repr(stream);
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
	Expr *index; // If accessible is array, save the accessed cell index

	AccessibleExpr(Token *id, Expr *index) :
		id(id),
		index(index)
	{
	}

	AccessibleExpr(Token *id) :
		AccessibleExpr(id, NULL)
	{
	}

	std::ostream &Repr(std::ostream &stream) const override
	{
		stream << *id;

		if (index != NULL)
		{
			stream << "[";
			index->Repr(stream);
			stream << "]";
		}

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
	std::vector<const Expr *> *values;

	ArrayExpr()
	{
		this->values = new std::vector<const Expr *>();
	}

	void Add(const Expr *expr)
	{
		this->values->push_back(expr);
	}

	std::ostream &Repr(std::ostream &stream) const override
	{
		stream << "[";

		for (size_t i = 0; i < values->size(); i++)
		{
			values->at(i)->Repr(stream);
			if (i < values->size() - 1) stream << ", ";
		}

		stream << "]";
		return stream;
	}

	void Accept(IVisitor *visitor) const override;
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

class InitExpr : public AccessibleExpr
{
public:
	Token *type;
	AssignExpr *assign;

	InitExpr(Token *type, Token *id, AssignExpr *assign) :
		type(type),
		AccessibleExpr(id),
		assign(assign)
	{
	}

	InitExpr(Token *type, Token *id) :
		InitExpr(type, id, NULL)
	{
	}

	std::ostream &Repr(std::ostream &stream) const override
	{
		stream << '(';
		stream << *type;
		stream << ' ';

		if (assign)
			assign->Repr(stream);
		else
			stream << *id;

		stream << ')';
		return stream;
	}

	void Accept(IVisitor *visitor) const override;
};

class IfExpr : public Expr
{
public:
	CondExpr *cond;
	ExprGroup *block;
	IfExpr *elif; // Chain elif expressions

	IfExpr(CondExpr *cond, ExprGroup *block, IfExpr *elif) :
		cond(cond),
		block(block),
		elif(elif)
	{
	}

	IfExpr(CondExpr *cond, ExprGroup *block) :
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
	ExprGroup *block;

	ElseExpr(IfExpr *ifExpr, ExprGroup *block) :
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
	ExprGroup *block;

	WhileExpr(CondExpr *cond, ExprGroup *block) :
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
	ExprGroup *block;

	ForExpr(Expr *assign, CondExpr *cond, Expr *incr, ExprGroup *block) :
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

class FuncExpr : public Expr
{
public:
	Token *type;
	Token *id;
	Expr *body;

	FuncExpr(Token *type, Token *id, Expr *body) :
		type(type),
		id(id),
		body(body)
	{
	}

	void Accept(IVisitor *visitor) const override;

	std::ostream &Repr(std::ostream &stream) const override
	{
		stream << *type;
		stream << " func ";
		stream << *id;
		stream << ":\n{\n";
		body->Repr(stream);
		stream << "\n}";
		return stream;
	}
};
