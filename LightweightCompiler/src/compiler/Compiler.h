#pragma once
#include <string>
#include <stack>
#include <vector>
#include "../parser/Expr.h"
#include "../tables/VarTable.h"
#include "../asm/ASMGenerator.h"
#include "../visitors/StatementVisitor.h"
#include "../visitors/ValueVisitor.h"
#include "../visitors/ControllableVisitor.h"

void ThrowCompileError(std::string error);

std::string Compile(const ExprGroup *block);
