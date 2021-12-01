#include "VarTable.h"

TypeTable::TypeTable()
{
	this->typeTable = std::unordered_map<std::string, Type *>();
}

Type *TypeTable::GetType(Token *typeId)
{
	/*switch (typeId->type)
	{
	case TokenType::INT:
		return SIZE_INT;

	case TokenType::FLOAT:
		return SIZE_FLOAT;

	case TokenType::BOOL:
		return SIZE_BOOL;

	case TokenType::CHAR:
		return SIZE_CHAR;
	}*/

	return NULL;
}

VarTable::VarTable()
{
	this->varMap = std::unordered_map<std::string, Var*>();
	this->totalBytes = 0;
}

int VarTable::GetNextOffset()
{
	return totalBytes;
}

void VarTable::Add(Var *var)
{
	varMap[var->id->literal] = var;
	totalBytes += var->typeSize;
}

Var *VarTable::Get(Token *id)
{
	auto iterator = varMap.find(id->literal);
	return iterator == varMap.end() ? NULL : iterator->second;
}
