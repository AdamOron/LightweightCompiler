#include "VarTable.h"

const Type *TypeTable::TYPE_INT		= new Type(4);
const Type *TypeTable::TYPE_FLOAT	= new Type(4);
const Type *TypeTable::TYPE_BOOL	= new Type(1);
const Type *TypeTable::TYPE_CHAR	= new Type(1);

TypeTable::TypeTable()
{
	this->typeTable = std::unordered_map<std::string, Type *>();
}

const Type *TypeTable::GetType(const Token *value)
{
	switch (value->type)
	{
	case TokenType::INT:
		return TYPE_INT;

	case TokenType::FLOAT:
		return TYPE_FLOAT;

	case TokenType::BOOL:
		return TYPE_BOOL;

	case TokenType::CHAR:
		return TYPE_CHAR;
	}

	return NULL;
}

VarTable::VarTable()
{
	this->types = TypeTable();
	this->varMap = std::unordered_map<std::string, Var*>();
	this->totalBytes = 0;
}

Var *VarTable::Add(const Token* id)
{
	Var *var = new Var(id, TypeTable::TYPE_INT, totalBytes + TypeTable::TYPE_INT->size);

	varMap[var->id->literal] = var;
	totalBytes += var->type->size;

	return var;
}

Var *VarTable::Get(const Token *id)
{
	auto iterator = varMap.find(id->literal);
	return iterator == varMap.end() ? NULL : iterator->second;
}
