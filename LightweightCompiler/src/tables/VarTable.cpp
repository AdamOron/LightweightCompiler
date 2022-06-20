#include "VarTable.h"

const Type *TypeTable::TYPE_INT		= new Type(4);
const Type *TypeTable::TYPE_FLOAT	= new Type(4);
const Type *TypeTable::TYPE_BOOL	= new Type(1);
const Type *TypeTable::TYPE_CHAR	= new Type(1);

TypeTable::TypeTable() : 
	typeTable(std::unordered_map<VarId, const Type *>())
{
}

const Type *TypeTable::GetType(const Token *type)
{
	switch (type->type)
	{
	case TokenType::TYPE_INT:
		return TYPE_INT;

	case TokenType::TYPE_FLOAT:
		return TYPE_FLOAT;

	case TokenType::TYPE_BOOL:
		return TYPE_BOOL;

	case TokenType::TYPE_CHAR:
		return TYPE_CHAR;
	}

	return NULL;
}

size_t VarTable::totalBytes = 0;

VarTable::VarTable()
{
	this->varMap = std::unordered_map<std::string, Var*>();
}

Var *VarTable::Add(const Token *id, const Type *type)
{
	if (varMap[id->literal] != NULL) return NULL;

	Var *var = new Var(id, type, totalBytes + type->size);

	totalBytes += type->size;
	varMap[var->id->literal] = var;

	return var;
}

Var *VarTable::Get(const Token *id)
{
	auto iterator = varMap.find(id->literal);
	return iterator == varMap.end() ? NULL : iterator->second;
}
