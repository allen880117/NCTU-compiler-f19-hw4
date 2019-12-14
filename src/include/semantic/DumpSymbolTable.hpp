#pragma once
#include "AST/ast.hpp"
#include "semantic/SymbolTable.hpp"

//
// TODO: Dump Function
//

string info_convert(VariableInfo input);

void dumpDemarcation(const char chr);
void dumpSymbol_Header();
void dumpSymbol_Body(SymbolEntry symbol_entry);
void dumpSymbol_Bottom();
