#include "semantic/SemanticAnalyzer.hpp"
#include "semantic/SymbolTable.hpp"
#include "semantic/ErrorMsg.hpp"
#include "semantic/DumpSymbolTable.hpp"
#include "AST/ast.hpp"
#include "AST/program.hpp"
#include "AST/declaration.hpp"
#include "AST/variable.hpp"
#include "AST/constant_value.hpp"
#include "AST/function.hpp"
#include "AST/compound_statement.hpp"
#include "AST/assignment.hpp"
#include "AST/print.hpp"
#include "AST/read.hpp"
#include "AST/variable_reference.hpp"
#include "AST/binary_operator.hpp"
#include "AST/unary_operator.hpp"
#include "AST/if.hpp"
#include "AST/while.hpp"
#include "AST/for.hpp"
#include "AST/return.hpp"
#include "AST/function_call.hpp"
#include <iostream>
#include <iomanip>
#include <cstdio>
using namespace std;

//
// TODO: implementations of constructor and destructor
//

SemanticAnalyzer::SemanticAnalyzer(string _filename, FILE* _fp, int _dump_enable){
    this->symbol_table_root = new SymbolTable(0);
    this->current_scope     = this->symbol_table_root;
    this->level             = 0;

    _filename = _filename.substr(0, _filename.length()-2);
    for(uint i=_filename.length()-1; i>=0; i--){
        if(_filename[i] == '/' ){
            _filename = _filename.substr(i+1, _filename.length()-i);
            break;
        }
    }
    this->filename = _filename;
    this->fp = _fp;
    this->dump_enable = _dump_enable;

    this->semantic_error = 0;
    this->error_msg = "";

    this->specify = false;
    this->specify_kind = KIND_UNKNOWN;

    this->compound_level_up_need = true;
}

void SemanticAnalyzer::level_up(){this->level++;}
void SemanticAnalyzer::level_down(){this->level--;}
void SemanticAnalyzer::output_err_msg(){cout<<this->error_msg;}
int  SemanticAnalyzer::is_semantic_error(){return this->semantic_error;}

SymbolTable* SemanticAnalyzer::get_symbol_table(){return this->symbol_table_root;}

void SemanticAnalyzer::push(SymbolTable* _new_scope){
    _new_scope->prev_scope = this->current_scope;
    this->current_scope->next_scope_list.push_back(_new_scope);
    this->current_scope = _new_scope;
}
void SemanticAnalyzer::pop(){
    this->current_scope = this->current_scope->prev_scope;
}

void SemanticAnalyzer::specify_on(FieldKind _field_kind){
    this->specify = true;
    this->specify_kind = _field_kind;
}

void SemanticAnalyzer::specify_off(){
    this->specify = false;
}

void SemanticAnalyzer::compound_level_up_need_on(){
    this->compound_level_up_need = true;
}

void SemanticAnalyzer::compound_level_up_need_off(){
    this->compound_level_up_need = false;
}