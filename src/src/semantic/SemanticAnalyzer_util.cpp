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

SemanticAnalyzer::SemanticAnalyzer(string _filename, FILE* _fp){
    this->symbol_table_root = new SymbolTable(0);
    this->current_scope     = this->symbol_table_root;
    this->level             = 0;

    _filename = _filename.substr(0, _filename.length()-2);
    for(int i=_filename.length()-1; i>=0; i--){
        if(_filename[i] == '/' ){
            _filename = _filename.substr(i+1, _filename.length()-i);
            break;
        }
    }
    this->filename = _filename;
    this->fp = _fp;

    this->semantic_error = 0;
    this->error_msg = "";

    this->specify = false;
    this->specify_kind = KIND_UNKNOWN;
}

void SemanticAnalyzer::level_up(){this->level++;}
void SemanticAnalyzer::level_down(){this->level--;}
void SemanticAnalyzer::output_err_msg(){cout<<this->error_msg;}
int  SemanticAnalyzer::is_semantic_error(){return this->semantic_error;}

SymbolTable* SemanticAnalyzer::get_symbol_table(){return this->symbol_table_root;}

void SemanticAnalyzer::dump_symbol_table(){
    this->dump_symbol_table_util(this->symbol_table_root->next_scope_list[0]);
}

void SemanticAnalyzer::push(SymbolTable* _new_scope, Node m, enum NODE_TABLE type,VariableInfo re_type){
    _new_scope->prev_scope = this->current_scope;
    _new_scope->prev_scope_node = m;
    _new_scope->prev_node_type = type;
    _new_scope->prev_return_type = re_type;
    this->current_scope->next_scope_list.push_back(_new_scope);
    this->current_scope->next_scope_node_list.push_back(m);
    this->current_scope->next_node_type_list.push_back(type);

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

void SemanticAnalyzer::push_src_node(NODE_TABLE _node){
    this->src_node.push(_node);
}

void SemanticAnalyzer::pop_src_node(){
    this->src_node.pop();
}

void SemanticAnalyzer::dump_symbol_table_util(SymbolTable* enter){
    
    for(uint i=0; i<enter->next_scope_list.size(); i++)
        dump_symbol_table_util(enter->next_scope_list[i]);

    dumpSymbol_Header();
        for(uint i=0; i<enter->entry_name.size(); i++)
            dumpSymbol_Body(enter->entry[enter->entry_name[i]]);
    dumpSymbol_Bottom();

}

// false -> not found
bool SemanticAnalyzer::check_symbol_inside(string _name){
    if(_name.length() > 32) _name = _name.substr(0,32);
    bool found = false;
    SymbolTable* current = this->current_scope;
    while(true){
        if(current->entry[_name].is_used == true){
            found = true;
            break;
        } else {
            if(current->level == 0) break;
            else current = current->prev_scope;
        }
    }
    return found;
}

SymbolEntry SemanticAnalyzer::get_symbol_entry(string _name){
    SymbolEntry temp;
    SymbolTable* current = this->current_scope;
    while(true){
        if(current->entry[_name].is_used == true){
            return current->entry[_name];
        } else {
            if(current->level == 0) break;
            else current = current->prev_scope;
        }
    }
    return temp; // Won't Happen .... Maybe
}

// true -> loop_var has been declared
bool SemanticAnalyzer::check_loop_var(string _name){
    if(_name.length() > 32) _name = _name.substr(0,32);
    bool found = false;
    SymbolTable* current = this->current_scope;
    while(true){
        if( current->entry[_name].is_used == true &&
            current->entry[_name].kind == KIND_LOOP_VAR )
        {
            found = true;
            break;
        } else {
            if(current->level == 0) break;
            else current = current->prev_scope;
        }
    }
    return found;
}

// true -> error array declaration
bool SemanticAnalyzer::check_array_declaration_error(string _name){
    if(_name.length() > 32) _name = _name.substr(0,32);
    bool is_error = false;
    SymbolTable* current = this->current_scope;
    while(true){
        if( current->entry[_name].is_used == true )
        {
            is_error = current->entry[_name].is_error;
            break;
        } else {
            if(current->level == 0) break;
            else current = current->prev_scope;
        }
    }
    return is_error;
}

// true -> inside program / procedure call
bool SemanticAnalyzer::check_program_or_procedure_call(){
    bool is_error = false;
    SymbolTable* current = this->current_scope;
    while(true){
        if(current->level == 1 && current->prev_scope->level == 0){
            if(current->prev_return_type.type == TYPE_VOID){
                return true;
            }   
            break;
        }
        else current = current->prev_scope;
    }
    return is_error;
}

VariableInfo SemanticAnalyzer::get_function_return_type(){
    SymbolTable* current = this->current_scope;
    while(true){
        if(current->level == 1 && current->prev_scope->level == 0){
            return current->prev_return_type;
        }
        else current = current->prev_scope;
    }
}

void SemanticAnalyzer::push_error_stack(bool tof){
    this->error_stack.push(tof);
}

bool SemanticAnalyzer::get_pop_error_stack(){
    bool tmp = this->error_stack.top();
    this->error_stack.pop();
    return tmp;
}