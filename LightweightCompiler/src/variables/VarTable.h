#pragma once
#include "../tokens/Token.h"
#include <unordered_map>

struct Type
{
	const size_t size;

	Type(const size_t typeSize) :
		size(typeSize)
	{
	}
};

using VarId = std::string;

class TypeTable
{
private:
	/* Map variable's IDs to their types */
	std::unordered_map<VarId, const Type *> typeTable;

public:
	/* I am currently just saving these 4 types as a temporary solution. This is obviously not elegant and doesn't really help with anything. */
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
	const size_t memOffset;

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
	static size_t totalBytes;
	TypeTable types;
	std::unordered_map<VarId, Var *> varMap;

public:
	VarTable();
	Var *Add(const Token *id, const Type *type);
	Var *Get(const Token *id);
};
