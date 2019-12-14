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
// TODO: implementations of visit(xxxxNode *)
//

void SemanticAnalyzer::visit(ProgramNode *m) {
    // Put Symbol Table (Special Case)
    SymbolTable* new_scope = new SymbolTable(0);
    this->push(new_scope);

    // Push Symbol Entity
    if(this->current_scope->redeclare_check(m->program_name) == false ){
        // Error: Redeclare
        this->semantic_error = 1;
        this->error_msg+=redeclare_error_msg(m->line_number, m->col_number, m->program_name);
        this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
        
    } else {
        VariableInfo tmpInfo;
        tmpInfo.type_set = UNKNOWN_SET;
        tmpInfo.type = TYPE_VOID;

        this->current_scope->put(
            SymbolEntry(
                m->program_name, 
                KIND_PROGRAM, 
                this->level, 
                tmpInfo, 
                Attribute(NO_ATTRIBUTE) 
            )
        );
    }

    // Visit Child Nodes
    this->push_src_node(PROGRAM_NODE);
        if (m->declaration_node_list != nullptr)
            for(uint i=0; i< m->declaration_node_list->size(); i++){
                (*(m->declaration_node_list))[i]->accept(*this);
            }

        if (m->function_node_list != nullptr)
            for(uint i=0; i< m->function_node_list->size(); i++){
                (*(m->function_node_list))[i]->accept(*this);
            }

        if (m->compound_statement_node != nullptr)
            m->compound_statement_node->accept(*this);
    this->pop_src_node();

    // Semantic Analyses of Program Node
    if(m->program_name != this->filename){
        this->semantic_error = 1;
        this->error_msg+=error_found_msg(m->line_number, m->col_number);
        this->error_msg+="program name must be the same as filename\n";
        this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
    }

    if(m->program_name != m->end_name){
        this->semantic_error = 1;
        this->error_msg+=error_found_msg(m->end_line_number, m->end_col_number);
        this->error_msg+="identifier at the end of program must be the same as identifier at the beginning of program\n";
        this->error_msg+=src_notation_msg(this->fp, m->end_line_number, m->end_col_number);
    }

    // Pop Scope
    this->pop();
}

void SemanticAnalyzer::visit(DeclarationNode *m) {
    // Visit Child Nodes
    this->push_src_node(DECLARATION_NODE);
        if (m->variables_node_list != nullptr)
            for(uint i=0; i< m->variables_node_list->size(); i++){
                (*(m->variables_node_list))[i]->accept(*this);
            }
    this->pop_src_node();
}

void SemanticAnalyzer::visit(VariableNode *m) {
    // Push Entry
    if(this->current_scope->redeclare_check(m->variable_name) == false ){
        // Error: Redeclare
        this->semantic_error = 1;
        this->error_msg+=redeclare_error_msg(m->line_number, m->col_number, m->variable_name);
        this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
        
    } else {
        if(this->specify == true){
            if(m->constant_value_node == nullptr){ // Not Constant
                this->current_scope->put(
                    SymbolEntry(
                        m->variable_name, 
                        this->specify_kind, 
                        this->level, 
                        *(m->type), 
                        Attribute(NO_ATTRIBUTE) 
                    )
                );
            }
            else {
                Attribute tempAttr(ATTRIBUTE_VALUE_OF_CONSTANT);
                tempAttr.set_value_of_constant(*(m->type));
                this->current_scope->put(
                    SymbolEntry(
                        m->variable_name, 
                        this->specify_kind, 
                        this->level, 
                        *(m->type), 
                        tempAttr
                    )
                );
            }
            
        } else {
            if(m->constant_value_node == nullptr){ // Not Constant
                this->current_scope->put(
                    SymbolEntry(
                        m->variable_name, 
                        KIND_VARIABLE, 
                        this->level, 
                        *(m->type), 
                        Attribute(NO_ATTRIBUTE) 
                    )
                );
            }
            else {
                Attribute tempAttr(ATTRIBUTE_VALUE_OF_CONSTANT);
                tempAttr.set_value_of_constant(*(m->type));
                this->current_scope->put(
                    SymbolEntry(
                        m->variable_name, 
                        KIND_CONSTANT, 
                        this->level, 
                        *(m->type), 
                        tempAttr
                    )
                );
            }
        }
        
    }

    // Semantic Check
}

void SemanticAnalyzer::visit(ConstantValueNode *m) {
    // Do Nothing
    ;
}

void SemanticAnalyzer::visit(FunctionNode *m) {
// Part 1:
    // Redeclare Check (current_scope still is global)
    if(this->current_scope->redeclare_check(m->function_name) == false){
        // Error: Redeclare
        this->semantic_error = 1;
        this->error_msg+=redeclare_error_msg(m->line_number, m->col_number, m->function_name);
        this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);

        return; // No need further check
    } else {
        // Push Name into global scope
        Attribute tempAttr(ATTRIBUTE_PARAMETERS);
        vector<VariableInfo> tempVI;
        for(uint i=0; i<m->prototype.size(); i++){tempVI.push_back(*(m->prototype[i]));}
        tempAttr.set_parameter_type(tempVI);

        this->current_scope->put(
            SymbolEntry(
                m->function_name,
                KIND_FUNCTION,
                this->level,
                *(m->return_type),
                tempAttr
            )
        );
    }

// Part 2:
    // Push Scope
    this->level_up();
    SymbolTable* new_scope = new SymbolTable(this->level);
    this->push(new_scope);

    // Visit Child Node
    this->push_src_node(FUNCTION_NODE);
        this->specify_on(KIND_PARAMETER);
            if (m->parameters != nullptr)
                for(uint i=0; i< m->parameters->size(); i++){
                    (*(m->parameters))[i]->node->accept(*this);
                }
        this->specify_off();
        
            if (m->body != nullptr)
                m->body->accept(*this);
    
    this->pop_src_node();
   
    // Semantic Check
    if (m->function_name != m->end_name){
        this->semantic_error = 1;
        this->error_msg+=error_found_msg(m->end_line_number, m->end_col_number);
        this->error_msg+="identifier at the end of function must be the same as identifier at the beginning of function\n";
        this->error_msg+=src_notation_msg(this->fp, m->end_line_number, m->end_col_number);
    }

    // Pop Scope
    this->pop();
    this->level_down();
}

void SemanticAnalyzer::visit(CompoundStatementNode *m) {
    // Push Scope
    if(this->src_node.top() == FUNCTION_NODE) this->level_up();

    // Pop Scope
    if(this->src_node.top() == FUNCTION_NODE) this->level_down();
}

void SemanticAnalyzer::visit(AssignmentNode *m) {}

void SemanticAnalyzer::visit(PrintNode *m) {}

void SemanticAnalyzer::visit(ReadNode *m) {}

void SemanticAnalyzer::visit(VariableReferenceNode *m) {}

void SemanticAnalyzer::visit(BinaryOperatorNode *m) {}

void SemanticAnalyzer::visit(UnaryOperatorNode *m) {}

void SemanticAnalyzer::visit(IfNode *m) {}

void SemanticAnalyzer::visit(WhileNode *m) {}

void SemanticAnalyzer::visit(ForNode *m) {}

void SemanticAnalyzer::visit(ReturnNode *m) {}

void SemanticAnalyzer::visit(FunctionCallNode *m) {}
