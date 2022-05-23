#pragma once
#include "../tokens/Token.h"
#include "Expr.h"

class Parser
{
private:
	std::vector<Token> &tokens;
	unsigned int index;
	int indentCount;

	/**
	* Retrieves the Token in the current index.
	* Doesn't verify that current index is valid.
	*
	* @return the current Token.
	*/
	Token &Current();
	/**
	* Progress to next Token by incrementing the index.
	*/
	void Next();
	/**
	* Return to previous Token by decrementing index.
	*/
	void Prev();
	/**
	* Checks whether the current index is valid.
	*
	* @return whether there's a current Token
	*/
	bool HasCurrent();
	/**
	* Comapres all given TokenTypes with current Token's TokenType. Returns whether any given TokenType matches.
	* Doesn't guarantee that there's a current Token.
	*
	* @param typeCount the amount of given TokenTypes
	* @param ... the desired TokenTypes
	* @return whether current Token's TokenType matches any of the given TokenTypes
	*/
	bool Match(int typeCount, ...);
	/**
	* Check whether current Token is a Type Token (TYPE_VOID, TYPE_INT, etc)
	*/
	bool IsType();
	/**
	* Checks & returns whether the next Token's TokenType matches any of the given TokenTypes.
	* If there's a match, the Parser advances to the next Token.
	* Doesn't guarantee that there's a next Token.
	*
	* @param typeCount the amount of given TokenTypes
	* @param ... the desired TokenTypes
	* @return whether next Token's TokenType matches any of the given TokenTypes
	*/
	bool MatchNext(int typeCount, ...);
	/**
	* Checks whether current Token's TokenType matches any of the given TokenTypes. If there's no match, an 'invalid_argument' exception is thrown.
	* Doesn't guarantee that there's a current Token.
	*
	* @param typeCount the amount of given TokenTypes
	* @param ... the desired TokenTypes
	*/
	void Expect(int typeCount, ...);
	bool HasIndents();
	void ExpectIndents();

public:
	Parser(std::vector<Token> &tokens);

	Expr *Atom();
	Expr *List();
	Expr *Primary();
	/**
	* On entry: current Token is INT/FLOAT or LPAREN.
	* On exit: current Token is either non-existant or an oper (non-unary).
	* Scans Tokens and returns a matching ValExpr/BinaryExpr of POWER.
	*
	* @return ValExpr/GroupExpr/BinaryExpr of POW
	*/
	Expr *Power();
	/**
	* On entry: current Token is ADD/SUB or INT/FLOAT.
	* On exit: current Token is either non-existant or an oper (non-unary).
	* Scans Tokens and returns a matching ValExpr/UnaryExpr.
	*
	* @return ValExpr/UnaryExpr
	*/
	Expr *Factor();
	/**
	* On entry: current Token matches Rank2 entry.
	* On exit: current Token is non-existant or ADD/SUB (non-unary).
	* Scans Tokens and returns a matching BinaryExpr of MULT/DIV.
	*
	* @return BinaryExpr of MULT/DIV
	*/
	Expr *Term();
	/**
	* On entry: current Token matches Rank1 entry.
	* On exit: current Token is non-existant.
	* Scans Tokens and returns a matching BinaryExpr of ADD/SUB.
	*
	* @return BinaryExpr of ADD/SUB
	*/
	Expr *Sum();
	Expr *Shift();
	Expr *BinaryAnd();
	Expr *BinaryXor();
	Expr *BinaryOr();
	Expr *Relation();
	Expr *Equality();
	Expr *And();
	Expr *Or();
	Expr *Ternary();
	Expr *ValueExpr();
	Expr *Assign();
	Expr *Init();
	Expr *Print();
	IfExpr *If();
	IfExpr *Elif();
	Expr *Else();
	Expr *Jump();
	Expr *While();
	Expr *For();
	Expr *Func();
	Expr *Statement();

	ExprGroup *DeepCodeBlock();
};

ExprGroup *ParseExprs(std::vector<Token>& tokens);
