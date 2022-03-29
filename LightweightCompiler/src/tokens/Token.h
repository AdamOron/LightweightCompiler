#pragma once
#include <iostream>
#include <string>

/* Enums representing all possible Token types */
enum class TokenType
{
	/* Brackets */
	LP, RP, // Parenthesis
	LS, RS, // Square Brackets
	LC, RC, // Curly Brackets

	/* Type Specifiers, for functions */
	TYPE_VOID, TYPE_INT, TYPE_FLOAT, TYPE_BOOL, TYPE_CHAR, // STR,

	/* Value Types */
	VOID, INT, FLOAT, BOOL, CHAR, // STR,

	/* Math Operators */
	ADD, SUB, MULT, DIV, MOD, POW,
	/* Logical Operators */
	EQEQ, NEQ, LESS, LEQ, GRTR, GEQ,
	/* Boolean Operators */
	AND, OR, NOT,
	/* Bitwise Operators */
	BAND, BOR, BNOT, BXOR, SHL, SHR,

	/* Assignment Operator */
	EQ,
	EQ_ADD, EQ_SUB, EQ_MULT, EQ_DIV, EQ_MOD, EQ_POW,
	EQ_BAND, EQ_BOR, EQ_BNOT, EQ_XOR, EQ_SHL, EQ_SHR,

	ID, // 50 index
	QMARK, COLON,
	SEMICOL, COMMA,
	IF, ELSE, ELIF,
	FOR, WHILE, BREAK, CONTINUE,
	RETURN, PRINT,
	APOST,

	/* Misc */
	INDENT, ENDL, INVALID
};

/* Struct representing a Token */
struct Token
{
	/* Constant Strings for true/false boolean values.
	   We save these values because they will be used elsewhere as well. */
	static const std::string FALSE_LITERAL;
	static const std::string TRUE_LITERAL;

	/* The type of this Token */
	TokenType type;
	/* Pointer to the literal of this Token. Can be null if Token has no literal */
	std::string literal;
};

/**
* @param strm is the ostream we print to.
* @param token is the Token we're printing.
*/
inline std::ostream &operator<<(std::ostream &strm, const Token &token)
{
	strm << "<'";
	strm << token.literal;
	strm << "'>";
	return strm;
}