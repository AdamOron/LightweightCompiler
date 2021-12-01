#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

/* Enums representing all possible Token types */
enum class TokenType
{
	/* Brackets */
	LP, RP, // Parenthesis
	LS, RS, // Square Brackets
	LC, RC, // Curly Brackets

	/* Value Types */
	INT, FLOAT, BOOL, CHAR, // STR,

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

	ID, // 44 index
	QMARK, COLON,
	SEMICOL, COMMA,
	IF, ELSE, ELIF,
	FOR, WHILE, BREAK, CONTINUE,
	PRINT, RETURN,
	APOST,

	/* Misc */
	INDENT, ENDL, INVALID
};

/* Struct representing a Token */
struct Token
{
	/* Constant Strings for true/false boolean values.
	   We save these values because they will be used elsewhere as well. */
	static const std::string FALSE_LIT;
	static const std::string TRUE_LIT;

	/* The type of this Token */
	TokenType type;
	/* Pointer to the literal of this Token. Can be null if Token has no literal */
	std::string literal;
};

/**
* @param strm is the ostream we print to.
* @param token is the Token we're printing.
*/
inline std::ostream& operator<<(std::ostream &strm, const Token &token)
{
	strm << "<'";
	strm << token.literal;
	strm << "'>";
	return strm;
}

class Lexer
{
private:
	/* TokenTypes for all reserved language keywords mapped by their strings */
	static const std::unordered_map<std::string, TokenType> KEYWORDS;
	/* Source code to tokenize */
	std::string *source;
	/* Current char's index */
	size_t index;
	/* Index of the first char of the current Token */
	size_t tokenStart;

	/**
	* @param type the Token's type.
	* @return Token with current char streak (from tokenStart to index) and given type.
	*/
	Token CreateToken(TokenType type);

	void StartToken();

	void Expect(int charCount, ...);

	/**
	* @return current char from source.
	*/
	char Current();

	/**
	* Advance to next char by increasing index.
	*/
	void Next();

	/**
	* Return to previous char by decreasing index.
	*/
	void Prev();

	/**
	* @return whether current char is out of range or not.
	*/
	bool HasCurrent();

	/**
	* @param ch is the desired char.
	* @return whether next char is given char.
	*
	* Checks whether next char is given char.
	* If there's a match, it advances to the next char.
	*/
	bool MatchNext(char ch);

	bool IsIndent();

	/**
	* Skips next whitespace chars.
	*/
	void SkipWhitespace();

	/**
	* @return numeric Token.
	*
	* This function should be called only when encountering a digit.
	*/
	Token ScanLiteral();

	Token ScanKeyword();

	Token ScanChar();
	/**
	* @return basic Token.
	*/
	Token ScanOperator();

	Token ScanIndent();

	/**
	* @return current Token.
	*
	* Scans & returns Token starting at current index.
	* When the function exits, the cur char is the last char in the scanned Token.
	*/
	Token ScanToken();

public:
	/**
	* Construct Lexer with given source.
	*/
	Lexer(std::string *source) :
		source(source),
		index(0),
		tokenStart(0)
	{
	}

	/**
	* @param src is the source to be tokenized.
	* @return list of Tokens in the source.
	*/
	std::vector<Token> ScanTokens();
};
