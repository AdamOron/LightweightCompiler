#pragma once
#include <string>
#include <unordered_map>
#include "VarTable.h"

struct Func
{
	std::string name;
	const Type *returnType;
};

class FuncTable
{
private:
	std::unordered_map<std::string, const Func *> funcTable;

public:
	void Add(const Func *);
	const Func *Get(std::string name);

	// Implement FuncTable to keep track of created funcs. Then implement func calls (tokens/exprs, idk)
};
