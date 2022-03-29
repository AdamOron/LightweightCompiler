#include "Tokenizer.h"
#include <sstream>
#include <stdarg.h>

/* Save String literals for True/False */
const std::string Token::FALSE_LITERAL = "false";
const std::string Token::TRUE_LITERAL = "true";

/* Map every keyword to its matching TokenType */
const std::unordered_map<std::string, TokenType> Tokenizer::KEYWORDS =
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
	{Token::FALSE_LITERAL, TokenType::BOOL},
	{Token::TRUE_LITERAL, TokenType::BOOL},

	{"void", TokenType::TYPE_VOID},
	{"int", TokenType::TYPE_INT},
	{"float", TokenType::TYPE_FLOAT},
	{"bool", TokenType::TYPE_BOOL},
	{"char", TokenType::TYPE_CHAR},
};

Tokenizer::Tokenizer(const std::string &source) :
	source(source),
	index(0),
	tokenStart(0)
{
}

Token Tokenizer::CreateToken(TokenType type)
{
	std::string literal = "";

	for (size_t i = tokenStart; i <= index; i++)
	{
		literal += source.at(i);
	}

	return Token{ type, literal };
}

void Tokenizer::StartToken()
{
	tokenStart = index;
}

void Tokenizer::Expect(int charCount, ...)
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

char Tokenizer::Current()
{
	return source.at(index);
}

void Tokenizer::Next()
{
	index++;
}

void Tokenizer::Prev()
{
	index--;
}

bool Tokenizer::HasCurrent()
{
	return index < source.length();
}

bool Tokenizer::MatchNext(char ch)
{
	Next();

	if (Current() == ch)
	{
		return true;
	}

	Prev();
	return false;
}

bool Tokenizer::IsIndent()
{
	return Current() == '\n' || Current() == '\t';
}

void Tokenizer::SkipWhitespace()
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

Token Tokenizer::ScanLiteral()
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

Token Tokenizer::ScanKeyword()
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

Token Tokenizer::ScanChar()
{
	Next(); // Skip apostrophe (')

	StartToken(); // Token literal should only contain single char, not apostrophe

	Token charToken = CreateToken(TokenType::CHAR); // Create Token with single char

	Next(); // Advance past char to where final apostrophe should be

	Expect(1, '\''); // Expect final apostrophe

	return charToken;
}

Token Tokenizer::ScanOperator()
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
	case ',':
		return CreateToken(TokenType::COMMA);

	case '\'':
		return ScanChar();
	}

	/* Return Keyword Token */
	return ScanKeyword();
}

Token Tokenizer::ScanIndent()
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

Token Tokenizer::ScanToken()
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

std::vector<Token> Tokenizer::ScanTokens()
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
