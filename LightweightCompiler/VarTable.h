#pragma once
#include "Lexer.h"
#include <unordered_map>

struct Type
{
	const size_t size;

	Type(const size_t typeSize) :
		size(typeSize)
	{
	}
};

class TypeTable
{
private:
	std::unordered_map<std::string, Type *> typeTable;

public:
	static const Type *TYPE_INT;
	static const Type *TYPE_FLOAT;
	static const Type *TYPE_BOOL;
	static const Type *TYPE_CHAR;

	TypeTable();
	const Type *GetType(const Token *value);
};

struct Var
{
	const Token *id;
	const Type *type;
	size_t memOffset;

	Var(const Token *id, const Type *type, size_t memOffset) :
		id(id),
		type(type),
		memOffset(memOffset)
	{
	}
};

class VarTable
{
private:
	TypeTable types;
	std::unordered_map<std::string, Var *> varMap;
	size_t totalBytes;

public:
	VarTable();
	Var *Add(const Token *id);
	Var *Get(const Token *id);
};
