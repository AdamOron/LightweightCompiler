#pragma once
#include "Token.h"
#include <vector>
#include <unordered_map>

class Tokenizer
{
private:
	/* TokenTypes for all reserved language keywords mapped by their strings */
	static const std::unordered_map<std::string, TokenType> KEYWORDS;

	/* Source code to tokenize */
	const std::string &source;
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
	/* Scan indentation ('\t'), as this language is space-sensitive */
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
	Tokenizer(const std::string &source);

	/**
	* @param src is the source to be tokenized.
	* @return list of Tokens in the source.
	*/
	std::vector<Token> ScanTokens();
};
