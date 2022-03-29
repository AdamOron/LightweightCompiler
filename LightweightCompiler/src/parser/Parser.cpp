#include "Parser.h"
#include "../variables/VarTable.h"
#include <stdarg.h>

/**
* Retrieves the Token in the current index.
* Doesn't verify that current index is valid.
*
* @return the current Token.
*/
Token& Parser::Current()
{
	return tokens[index];
}

/**
* Progress to next Token by incrementing the index.
*/
void Parser::Next()
{
	index++;
}

/**
* Return to previous Token by decrementing index.
*/
void Parser::Prev()
{
	index--;
}

/**
* Checks whether the current index is valid.
*
* @return whether there's a current Token
*/
bool Parser::HasCurrent()
{
	return index < tokens.size();
}

/**
* Comapres all given TokenTypes with current Token's TokenType. Returns whether any given TokenType matches.
* Doesn't guarantee that there's a current Token.
*
* @param typeCount the amount of given TokenTypes
* @param ... the desired TokenTypes
* @return whether current Token's TokenType matches any of the given TokenTypes
*/
bool Parser::Match(int typeCount, ...)
{
	va_list iter;
	va_start(iter, typeCount);

	TokenType curType = Current().type;

	for (int i = 0; i < typeCount; i++)
	{
		TokenType iterType = va_arg(iter, TokenType);

		if (curType == iterType)
		{
			return true;
		}
	}

	return false;
}

bool Parser::IsType()
{
	return Match(5, TokenType::TYPE_VOID, TokenType::TYPE_INT, TokenType::TYPE_FLOAT, TokenType::TYPE_BOOL, TokenType::TYPE_CHAR);
}

/**
* Checks & returns whether the next Token's TokenType matches any of the given TokenTypes.
* If there's a match, the Parser advances to the next Token.
* Doesn't guarantee that there's a next Token.
*
* @param typeCount the amount of given TokenTypes
* @param ... the desired TokenTypes
* @return whether next Token's TokenType matches any of the given TokenTypes
*/
bool Parser::MatchNext(int typeCount, ...)
{
	Next();

	if (!HasCurrent()) return false;

	va_list iter;
	va_start(iter, typeCount);

	TokenType curType = Current().type;

	for (int i = 0; i < typeCount; i++)
	{
		TokenType iterType = va_arg(iter, TokenType);

		if (curType == iterType)
		{
			return true;
		}
	}

	Prev();
	return false;
}

/**
* Checks whether current Token's TokenType matches any of the given TokenTypes. If there's no match, an 'invalid_argument' exception is thrown.
* Doesn't guarantee that there's a current Token.
*
* @param typeCount the amount of given TokenTypes
* @param ... the desired TokenTypes
*/
void Parser::Expect(int typeCount, ...)
{
	if (!HasCurrent())
	{
		std::cout << "Unexpected EOF.";
		exit(1);
	}

	va_list iter;
	va_start(iter, typeCount);

	TokenType curType = Current().type;

	for (int i = 0; i < typeCount; i++)
	{
		TokenType iterType = va_arg(iter, TokenType);

		if (curType == iterType)
		{
			return;
		}
	}

	std::cout << "Unexpected " << Current() << " Token, index " << index << ".";
	exit(1);
}

bool Parser::HasIndents()
{
	if (index + indentCount >= tokens.size()) return false;

	for (size_t i = index; i < index + indentCount; i++)
	{
		if (tokens.at(i).type != TokenType::INDENT) return false;
	}

	return true;
}

void Parser::ExpectIndents()
{
	for (int i = 0; i < indentCount; i++)
	{
		Expect(1, TokenType::INDENT);
		Next();
	}
}

Parser::Parser(std::vector<Token> &tokens) :
	tokens(tokens),
	index(0),
	indentCount(-1)
{
}

Expr* Parser::Atom()
{
	if (Match(1, TokenType::ID))
	{
		Token *id = &Current();

		if (MatchNext(1, TokenType::LS))
		{
			Next();

			Expr *index = ValueExpr();

			Next();
			Expect(1, TokenType::RS);

			return new AccessibleExpr(id, index);
		}

		return new AccessibleExpr(id);
	}

	Expect(3, TokenType::INT, TokenType::FLOAT, TokenType::BOOL);
	return new LitExpr(&Current());
}

Expr* Parser::List()
{
	if (!Match(1, TokenType::LS))
	{
		return Atom();
	}

	Next();

	ArrayExpr *arrayExpr = new ArrayExpr();

	while (true)
	{
		if (Match(1, TokenType::RS))
		{
			break;
		}

		arrayExpr->Add(ValueExpr());

		Next();

		if (Match(1, TokenType::COMMA))
		{
			Next();
		}
	}

	return arrayExpr;
}

Expr* Parser::Primary()
{
	if (Match(1, TokenType::LP))
	{
		Next();
		Expr* value = ValueExpr();

		Next();
		Expect(1, TokenType::RP);

		return new GroupExpr(value);
	}

	return List();
}

/**
* On entry: current Token is INT/FLOAT or LPAREN.
* On exit: current Token is either non-existant or an oper (non-unary).
* Scans Tokens and returns a matching ValExpr/BinaryExpr of POWER.
*
* @return ValExpr/GroupExpr/BinaryExpr of POW
*/
Expr* Parser::Power()
{
	Expr* left = Primary();

	if (MatchNext(1, TokenType::POW))
	{
		Token* oper = &Current();

		Next();
		Expr* right = Factor();

		return new BinaryExpr(left, right, oper);
	}

	return left;
}

/**
* On entry: current Token is ADD/SUB or INT/FLOAT.
* On exit: current Token is either non-existant or an oper (non-unary).
* Scans Tokens and returns a matching ValExpr/UnaryExpr.
*
* @return ValExpr/UnaryExpr
*/
Expr* Parser::Factor()
{
	if (Match(4, TokenType::ADD, TokenType::SUB, TokenType::NOT, TokenType::BNOT))
	{
		Token* oper = &Current();

		Next();
		Expr* right = Factor();

		return new UnaryExpr(oper, right);
	}

	return Power();
}

/**
* On entry: current Token matches Rank2 entry.
* On exit: current Token is non-existant or ADD/SUB (non-unary).
* Scans Tokens and returns a matching BinaryExpr of MULT/DIV.
*
* @return BinaryExpr of MULT/DIV
*/
Expr* Parser::Term()
{
	Expr* left = Factor();

	while (MatchNext(3, TokenType::MULT, TokenType::DIV, TokenType::MOD))
	{
		Token* oper = &Current();

		Next();

		Expr* right = Factor();

		left = new BinaryExpr(left, right, oper);
	}

	return left;
}

/**
* On entry: current Token matches Rank1 entry.
* On exit: current Token is non-existant.
* Scans Tokens and returns a matching BinaryExpr of ADD/SUB.
*
* @return BinaryExpr of ADD/SUB
*/
Expr* Parser::Sum()
{
	Expr* left = Term();

	while (MatchNext(2, TokenType::ADD, TokenType::SUB))
	{
		Token* oper = &Current();

		Next();

		Expr* right = Term();
		left = new BinaryExpr(left, right, oper);
	}

	return left;
}

Expr* Parser::Shift()
{
	Expr* left = Sum();

	while (MatchNext(2, TokenType::SHL, TokenType::SHR))
	{
		Token* oper = &Current();

		Next();

		Expr* right = Sum();
		left = new BinaryExpr(left, right, oper);
	}

	return left;
}

Expr* Parser::BinaryAnd()
{
	Expr* left = Shift();

	while (MatchNext(1, TokenType::BAND))
	{
		Token* oper = &Current();

		Next();

		Expr* right = Shift();
		left = new BinaryExpr(left, right, oper);
	}

	return left;
}

Expr* Parser::BinaryXor()
{
	Expr* left = BinaryAnd();

	while (MatchNext(1, TokenType::BXOR))
	{
		Token* oper = &Current();

		Next();

		Expr* right = BinaryAnd();
		left = new BinaryExpr(left, right, oper);
	}

	return left;
}

Expr* Parser::BinaryOr()
{
	Expr* left = BinaryXor();

	while (MatchNext(1, TokenType::BOR))
	{
		Token* oper = &Current();

		Next();

		Expr* right = BinaryXor();
		left = new BinaryExpr(left, right, oper);
	}

	return left;
}

Expr* Parser::Relation()
{
	Expr* left = BinaryOr();

	while (MatchNext(4, TokenType::GRTR, TokenType::GEQ, TokenType::LESS, TokenType::LEQ))
	{
		Token* oper = &Current();

		Next();

		Expr* right = BinaryOr();
		left = new BinaryExpr(left, right, oper);
	}

	return left;
}

Expr* Parser::Equality()
{
	Expr* left = Relation();

	while (MatchNext(2, TokenType::EQEQ, TokenType::NEQ))
	{
		Token* oper = &Current();

		Next();

		Expr* right = Relation();
		left = new BinaryExpr(left, right, oper);
	}

	return left;
}

Expr* Parser::And()
{
	Expr* left = Equality();

	while (MatchNext(1, TokenType::AND))
	{
		Token* oper = &Current();

		Next();

		Expr* right = Equality();
		left = new BinaryExpr(left, right, oper);
	}

	return left;
}

Expr* Parser::Or()
{
	Expr* left = And();

	while (MatchNext(1, TokenType::OR))
	{
		Token* oper = &Current();

		Next();

		Expr* right = And();
		left = new BinaryExpr(left, right, oper);
	}

	return left;
}

Expr* Parser::Ternary()
{
	Expr* left = Or();

	if (MatchNext(1, TokenType::QMARK))
	{
		Next();

		Expr* e1 = Ternary();

		Next();
		Expect(1, TokenType::COLON);
		Next();

		Expr* e2 = Ternary();

		return new TernExpr(left, e1, e2);
	}

	return left;
}

Expr* Parser::ValueExpr()
{
	return Ternary();
}

Expr* Parser::LeftHand()
{
	return ValueExpr();
}

Expr *Parser::Init()
{
	if (!IsType())
	{
		return Assign();
	}

	Token *type = &Current();
	Next();

	Expect(1, TokenType::ID);
	Token *id = &Current();
	Next();

	Expect(1, TokenType::EQ);
	Next();

	Expr *value = Init();

	return new InitExpr(type, id, value);
}

Expr *Parser::Assign()
{
	Expr *left = LeftHand();

	if (MatchNext(13, TokenType::EQ, TokenType::EQ_ADD, TokenType::EQ_SUB,
		TokenType::EQ_MULT, TokenType::EQ_DIV, TokenType::EQ_MOD,
		TokenType::EQ_POW, TokenType::EQ_BAND, TokenType::EQ_BOR,
		TokenType::EQ_BNOT, TokenType::EQ_XOR, TokenType::EQ_SHL, TokenType::EQ_SHR))
	{
		Prev();
		Expect(1, TokenType::ID);
		Next();

		Token *assignOper = &Current();

		Next();

		Expr *value = ValueExpr();
		left = new AssignExpr((AccessibleExpr *) left, assignOper, value);
	}

	return left;
}

Expr* Parser::Print()
{
	if (Match(1, TokenType::PRINT))
	{
		Next();
		Expect(1, TokenType::LP);
		Next();

		Expr* value = ValueExpr();

		Next();
		Expect(1, TokenType::RP);

		return new PrintExpr(value);
	}

	return Init();
}

IfExpr* Parser::If()
{
	Next();

	CondExpr *cond = new CondExpr(LeftHand());

	Next();
	Expect(1, TokenType::ENDL);
	Next();

	ExprGroup *block = DeepCodeBlock();

	return new IfExpr(cond, block);
}

IfExpr* Parser::Elif()
{
	IfExpr* ifExpr = If();

	/* Keep track of the ending of last found IfExpr */
	int ifEnd = index;

	/* Used to iterate over IfExpr and construct elif chain */
	IfExpr* iter = ifExpr;

	while (true)
	{
		/* Move to (expected) EOL from last Token of IfExpr */
		Next();
		/* If no EOL, assume EOF and exit */
		if (!HasCurrent()) break;
		/* Expect EOL */
		Expect(1, TokenType::ENDL);
		/* Move to next statement */
		Next();
		/* If there isn't engouh indentation, there will be no ELIF */
		if (!HasIndents()) break;
		/* Expect the indentation */
		ExpectIndents();

		/* Check if current Token is ELIF */
		if (!Match(1, TokenType::ELIF)) break;

		/* Scan ELIF as if it were IF (same Exprs, different compiling methods) */
		IfExpr* elif = If();
		/* Add expr to elif chain & advance iter */
		iter->elif = elif;
		iter = iter->elif;
		/* Update ending of last found IfExpr */
		ifEnd = index;
	}

	/* Jump back to ending of last found IfExpr, ensuring we aren't skipping anything. */
	/* This is required as we aren't guaranteed to find ELIF, yet we skip Tokens to find it. */
	index = ifEnd;

	return ifExpr;
}

Expr* Parser::Else()
{
	if (!Match(1, TokenType::IF))
	{
		return Print();
	}

	/* Scan if/elif chain */
	IfExpr* ifExpr = Elif();

	int backup = index;

	/* Move to (expected) EOL from last Token of IfExpr */
	Next();
	/* If no EOL, assume EOF and exit */
	if (HasCurrent())
	{
		/* Expect EOL */
		Expect(1, TokenType::ENDL);
		/* Move to next statement */
		Next();
		/* If there isn't engouh indentation, there will be no ELIF */
		if (HasIndents())
		{
			/* Expect the indentation */
			ExpectIndents();

			if (Match(1, TokenType::ELSE))
			{
				Next();
				Expect(1, TokenType::ENDL);
				Next();

				ExprGroup * ifElse = DeepCodeBlock();

				return new ElseExpr(ifExpr, ifElse);
			}
		}
	}

	index = backup;
	return ifExpr;
}

Expr* Parser::Jump()
{
	if (!Match(2, TokenType::BREAK, TokenType::CONTINUE))
	{
		return Else();
	}

	return new ControlFlowExpr(&Current());
}

Expr* Parser::While()
{
	if (!Match(1, TokenType::WHILE))
	{
		return Jump();
	}

	Next();

	CondExpr* cond = new CondExpr(LeftHand());

	Next();
	Expect(1, TokenType::ENDL);
	Next();

	ExprGroup * block = DeepCodeBlock();

	return new WhileExpr(cond, block);
}

Expr* Parser::For()
{
	if (!Match(1, TokenType::FOR))
	{
		return While();
	}

	Next();
	Expr* assign = Init();
	Next();
	Expect(1, TokenType::COMMA);
	Next();

	CondExpr* cond = new CondExpr(LeftHand());
	Next();
	Expect(1, TokenType::COMMA);
	Next();

	Expr* incr = Assign();
	Next();
	Expect(1, TokenType::ENDL);
	Next();

	ExprGroup * block = DeepCodeBlock();

	return new ForExpr(assign, cond, incr, block);
}

// Not being used currently
Expr *Parser::Func()
{
	if (!Match(5, TokenType::TYPE_VOID, TokenType::TYPE_INT, TokenType::TYPE_FLOAT, TokenType::TYPE_BOOL, TokenType::TYPE_CHAR))
	{
		return While();
	}

	Token *type = &Current();
	Next();

	Expect(1, TokenType::ID);
	Token *id = &Current();
	Next();

	Expect(1, TokenType::LP);
	Next();
	Expect(1, TokenType::RP);
	Next();

	Expect(1, TokenType::ENDL);
	Next();

	ExprGroup *body = DeepCodeBlock();

	return new FuncExpr(type, id, body);
}

Expr* Parser::Statement()
{
	return For();
}

ExprGroup* Parser::DeepCodeBlock()
{
 	indentCount++;

	ExprGroup* block = new ExprGroup();

	while (HasCurrent())
	{
		if (!HasIndents())
		{
			Prev(); Prev();
			break;
		}

		ExpectIndents();

		if (Match(1, TokenType::ENDL))
		{
			Next();
			continue;
		}

		block->Add(Statement());
		Next();

		if (!HasCurrent()) break;

		Expect(1, TokenType::ENDL);
		Next();
	}

	indentCount--;

	return block;
}

ExprGroup* ParseExprs(std::vector<Token>& tokens)
{
	Parser parser(tokens);
	return parser.DeepCodeBlock();
}
