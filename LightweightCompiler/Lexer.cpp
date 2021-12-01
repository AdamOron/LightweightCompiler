#include "Lexer.h"
#include <sstream>
#include <stdarg.h>

const std::string Token::FALSE_LIT = "false";
const std::string Token::TRUE_LIT = "true";

const std::unordered_map<std::string, TokenType> Lexer::KEYWORDS =
{
	{"if", TokenType::IF},
	{"else", TokenType::ELSE},
	{"elif", TokenType::ELIF},
	{"for", TokenType::FOR},
	{"while", TokenType::WHILE},
	{"break", TokenType::BREAK},
	{"continue", TokenType::CONTINUE},
	{"print", TokenType::PRINT},
	{"return", TokenType::RETURN},
	{Token::FALSE_LIT, TokenType::BOOL},
	{Token::TRUE_LIT, TokenType::BOOL},
};

/**
* @param type the Token's type.
* @return Token with current char streak (from tokenStart to index) and given type.
*/
Token Lexer::CreateToken(TokenType type)
{
	std::string literal = "";

	for (size_t i = tokenStart; i <= index; i++)
	{
		literal += source->at(i);
	}

	return Token{ type, literal };
}

void Lexer::StartToken()
{
	tokenStart = index;
}

void Lexer::Expect(int charCount, ...)
{
	if (!HasCurrent())
	{
		std::cout << "Unexpected EOF.";
		exit(1);
	}

	va_list iter;
	va_start(iter, charCount);

	char curChar = Current();

	for (int i = 0; i < charCount; i++)
	{
		char iterChar  = va_arg(iter, char);

		if (curChar == iterChar)
		{
			return;
		}
	}

	std::cout << "Unexpected " << Current() << " at index " << index << ".";
	exit(1);
}

/**
* @return current char from source.
*/
char Lexer::Current()
{
	return source->at(index);
}

/**
* Advance to next char by increasing index.
*/
void Lexer::Next()
{
	index++;
}

/**
* Return to previous char by decreasing index.
*/
void Lexer::Prev()
{
	index--;
}

/**
* @return whether current char is out of range or not.
*/
bool Lexer::HasCurrent()
{
	return index < source->length();
}

/**
* @param ch is the desired char.
* @return whether next char is given char.
*
* Checks whether next char is given char.
* If there's a match, it advances to the next char.
*/
bool Lexer::MatchNext(char ch)
{
	Next();

	if (Current() == ch)
	{
		return true;
	}

	Prev();
	return false;
}

bool Lexer::IsIndent()
{
	return Current() == '\n' || Current() == '\t';
}

/**
* Skips next whitespace chars.
*/
void Lexer::SkipWhitespace()
{
	/* As long as the current char is valid */
	while (HasCurrent())
	{
		/* If the current char isn't whitespace, exit the function */
		if (!isspace(Current()) || IsIndent())
		{
			break;
		}

		/* If current char is whitespace, skip it */
		Next();
	}
}

/**
* @return numeric Token.
*
* This function should be called only when encountering a digit.
*/
Token Lexer::ScanLiteral()
{
	/* Represents whether the number is decimal or not */
	bool isDecimal = false;

	Next();

	while (HasCurrent())
	{
		char cur = Current();

		/* If current char isn't a digit (will also run on null char) */
		if (!isdigit(cur))
		{
			/* If it's not the first decimal point */
			if (cur != '.' || isDecimal)
			{
				/* Break the loop */
				break;
			}

			/* If it is the first decimal point, set isDecimal to true */
			isDecimal = true;
		}

		Next();
	}

	/* The loop is exited when an invalid char is encountered, so we return to the previous char. */
	Prev();

	/* Create Token with a FLOAT type incase it's decimal and an INT type otherwise, and the number's literal */
	return CreateToken(isDecimal ? TokenType::FLOAT : TokenType::INT);
}

Token Lexer::ScanKeyword()
{
	std::string keywordLiteral = "";
	keywordLiteral += Current();

	Next();

	while (HasCurrent())
	{
		char current = Current();

		if (!isalpha(current) && !isdigit(current))
		{
			break;
		}

		keywordLiteral += current;

		Next();
	}

	Prev();

	auto iterator = KEYWORDS.find(keywordLiteral);
	return CreateToken(iterator == KEYWORDS.end() ? TokenType::ID : iterator->second);
}

Token Lexer::ScanChar()
{
	Next(); // Skip apostrophe (')

	StartToken(); // Token literal should only contain single char, not apostrophe

	Token charToken = CreateToken(TokenType::CHAR); // Create Token with single char

	Next(); // Advance past char to where final apostrophe should be

	Expect(1, '\''); // Expect final apostrophe

	return charToken;
}

/**
* @return basic Token.
*/
Token Lexer::ScanOperator()
{
	switch (Current())
	{
	/* Brackets */
	case '(':
		return CreateToken(TokenType::LP);
	case ')':
		return CreateToken(TokenType::RP);
	case '[':
		return CreateToken(TokenType::LS);
	case ']':
		return CreateToken(TokenType::RS);
	case '{':
		return CreateToken(TokenType::LC);
	case '}':
		return CreateToken(TokenType::RC);

	/* Operators */
	case '+':
		if (MatchNext('=')) return CreateToken(TokenType::EQ_ADD);
		return CreateToken(TokenType::ADD);
	case '-':
		if (MatchNext('=')) return CreateToken(TokenType::EQ_SUB);
		return CreateToken(TokenType::SUB);
	case '*':
		if (MatchNext('=')) return CreateToken(TokenType::EQ_MULT);
		return MatchNext('*') ? CreateToken(TokenType::POW) : CreateToken(TokenType::MULT);
	case '/':
		if (MatchNext('=')) return CreateToken(TokenType::EQ_DIV);
		return CreateToken(TokenType::DIV);
	case '%':
		if (MatchNext('=')) return CreateToken(TokenType::EQ_MOD);
		return CreateToken(TokenType::MOD);
	case '~':
		if (MatchNext('=')) return CreateToken(TokenType::EQ_BNOT);
		return CreateToken(TokenType::BNOT);
	case '^':
		if (MatchNext('=')) return CreateToken(TokenType::EQ_XOR);
		return CreateToken(TokenType::BXOR);
	case '&':
		if (MatchNext('=')) return CreateToken(TokenType::EQ_BAND);
		return MatchNext('&') ? CreateToken(TokenType::AND) : CreateToken(TokenType::BAND);
	case '|':
		if (MatchNext('=')) return CreateToken(TokenType::EQ_BOR);
		return MatchNext('|') ? CreateToken(TokenType::OR) : CreateToken(TokenType::BOR);
	case '=':
		return MatchNext('=') ? CreateToken(TokenType::EQEQ) : CreateToken(TokenType::EQ);
	case '!':
		return MatchNext('=') ? CreateToken(TokenType::NEQ) : CreateToken(TokenType::NOT);
	case '<':
		if (MatchNext('<')) return MatchNext('=') ? CreateToken(TokenType::EQ_SHL) : CreateToken(TokenType::SHL);
		return MatchNext('=') ? CreateToken(TokenType::LEQ) : CreateToken(TokenType::LESS);
	case '>':
		if (MatchNext('>')) return MatchNext('=') ? CreateToken(TokenType::EQ_SHR) : CreateToken(TokenType::SHR);
		return MatchNext('=') ? CreateToken(TokenType::GEQ) : CreateToken(TokenType::GRTR);
	case '?':
		return CreateToken(TokenType::QMARK);
	case ':':
		return CreateToken(TokenType::COLON);

	case '\'':
		return ScanChar();
	}

	/* Return Keyword Token */
	return ScanKeyword();
}

Token Lexer::ScanIndent()
{
	char indent = Current();

	switch (indent)
	{
	case '\n':
		return CreateToken(TokenType::ENDL);

	case '\t':
		return CreateToken(TokenType::INDENT);
	}

	return CreateToken(TokenType::INVALID);
}

/**
* @return current Token.
*
* Scans & returns Token starting at current index.
* When the function exits, the cur char is the last char in the scanned Token.
*/
Token Lexer::ScanToken()
{
	/* Save current Token starting index */
	StartToken();

	if (IsIndent())
	{
		return ScanIndent();
	}

	/* If the curent char is a digit */
	if (isdigit(Current()))
	{
		/* Return a number Token */
		return ScanLiteral();
	}

	/* Return basic Token if no other type found */
	return ScanOperator();
}

/**
* @param src is the source to be tokenized.
* @return list of Tokens in the source.
*/
std::vector<Token> Lexer::ScanTokens()
{
	std::vector<Token> tokens = std::vector<Token>();

	/* As long as there's a valid char to scan */
	while (HasCurrent())
	{
		/* Skip current whitespace chars */
		SkipWhitespace();

		/* If reached end, exit the loop */
		if (!HasCurrent())
		{
			break;
		}

		/* Scan current Token and save it */
		tokens.push_back(ScanToken());

		/* Advance past scanned Token */
		Next();
	}

	return tokens;
}
