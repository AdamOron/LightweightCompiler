#pragma once
#include "Lexer.h"
#include <unordered_map>

struct Type
{
	const Token type;
	const size_t typeSize;

	Type(const Token type, const size_t typeSize) :
		type(type),
		typeSize(typeSize)
	{
	}
};

class TypeTable
{
private:
	static const size_t SIZE_INT = 16;
	static const size_t SIZE_FLOAT = 16;
	static const size_t SIZE_BOOL = 1;
	static const size_t SIZE_CHAR = 1;

	std::unordered_map<std::string, Type *> typeTable;

public:
	TypeTable();
	Type *GetType(Token *typeId);
};

struct Var
{
	Token *id;
	int typeSize;
	int memOffset;

	Var(Token *id, int typeSize, int memOffset)
	{
		this->id = id;
		this->typeSize = typeSize;
		this->memOffset = memOffset;
	}
};

class VarTable
{
private:
	std::unordered_map<std::string, Var *> varMap;
	int totalBytes;

public:
	VarTable();
	int GetNextOffset();
	void Add(Var *var);
	Var *Get(Token *id);
};
