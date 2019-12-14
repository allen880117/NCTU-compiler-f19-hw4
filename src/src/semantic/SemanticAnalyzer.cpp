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

        SymbolEntry tmpEntry(
            m->program_name, 
            KIND_PROGRAM, 
            this->level, 
            tmpInfo, 
            Attribute(NO_ATTRIBUTE),
            PROGRAM_NODE
        );
        tmpEntry.program_node = m;

        this->current_scope->put(tmpEntry);
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
    // New Loop Var check Old Loop Var
    if(this->specify == true && this->specify_kind == KIND_LOOP_VAR ){
        if( check_loop_var(m->variable_name) == true ){
            // Loop Var has been declared
            // Error: Redeclare
            this->semantic_error = 1;
            this->error_msg+=redeclare_error_msg(m->line_number, m->col_number, m->variable_name);
            this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
            return;
        }
    }

    // New Variable check Old Loop Var
    if(check_loop_var(m->variable_name) == true ){
        // Error: Redeclare
        this->semantic_error = 1;
        this->error_msg+=redeclare_error_msg(m->line_number, m->col_number, m->variable_name);
        this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
        return;
    } 

    // New Variable check Redeclared
    if(this->current_scope->redeclare_check(m->variable_name) == false ){
        // Error: Redeclare
        this->semantic_error = 1;
        this->error_msg+=redeclare_error_msg(m->line_number, m->col_number, m->variable_name);
        this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
        return;
    } 

    if(this->specify == true){
        if(m->constant_value_node == nullptr){ // Not Constant
            SymbolEntry tmpEntry(
                m->variable_name, 
                this->specify_kind, 
                this->level, 
                *(m->type), 
                Attribute(NO_ATTRIBUTE),
                VARIABLE_NODE
            );
            tmpEntry.variable_node = m;
            this->current_scope->put(tmpEntry);
        }
        else {
            Attribute tempAttr(ATTRIBUTE_VALUE_OF_CONSTANT);
            tempAttr.set_value_of_constant(*(m->type));
            SymbolEntry tmpEntry(
                m->variable_name, 
                this->specify_kind, 
                this->level, 
                *(m->type), 
                tempAttr,
                VARIABLE_NODE
            );
            tmpEntry.variable_node = m;
            this->current_scope->put(tmpEntry);
        }
    } else {
        if(m->constant_value_node == nullptr){ // Not Constant
            SymbolEntry tmpEntry(
                m->variable_name, 
                KIND_VARIABLE, 
                this->level, 
                *(m->type), 
                Attribute(NO_ATTRIBUTE),
                VARIABLE_NODE
            );
            tmpEntry.variable_node = m;
            this->current_scope->put(tmpEntry);
        }
        else {
            Attribute tempAttr(ATTRIBUTE_VALUE_OF_CONSTANT);
            tempAttr.set_value_of_constant(*(m->type));
            SymbolEntry tmpEntry(
                m->variable_name, 
                KIND_CONSTANT,
                this->level, 
                *(m->type), 
                tempAttr,
                VARIABLE_NODE
            );
            tmpEntry.variable_node = m;
            this->current_scope->put(tmpEntry);
        }
    }

    // Semantic Check
    if(m->type->type_set == SET_ACCUMLATED){
        bool is_upperbound_le_lowerbound = false;
        for(uint i=0; i<m->type->array_range.size(); i++){
            if(m->type->array_range[i].end <= m->type->array_range[i].start){
                is_upperbound_le_lowerbound = true;
                break;
            }
        }

        if(is_upperbound_le_lowerbound == true){
            this->semantic_error = true;
            this->error_msg+=error_found_msg(m->line_number, m->col_number);
            this->error_msg+="'"+m->variable_name+"'";
            this->error_msg+=" declared as an array with a lower bound greater or equal to upper bound\n";
            this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
        }
    }
}

void SemanticAnalyzer::visit(ConstantValueNode *m) { //EXPRESSION
    this->expression_stack.push(*(m->constant_value));
}

void SemanticAnalyzer::visit(FunctionNode *m) { 
// Part 1:
    // Redeclare Check (current_scope still is global)
    if(this->current_scope->redeclare_check(m->function_name) == false){
        // Error: Redeclare
        this->semantic_error = 1;
        this->error_msg+=redeclare_error_msg(m->line_number, m->col_number, m->function_name);
        this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
    } else {
        // Push Name into global scope
        Attribute tempAttr(ATTRIBUTE_PARAMETERS);
        vector<VariableInfo> tempVI;
        for(uint i=0; i<m->prototype.size(); i++){tempVI.push_back(*(m->prototype[i]));}
        tempAttr.set_parameter_type(tempVI);

        SymbolEntry tmpEntry(
            m->function_name,
            KIND_FUNCTION,
            this->level,
            *(m->return_type),
            tempAttr,
            FUNCTION_NODE
        );
        tmpEntry.function_node = m;
        this->current_scope->put(tmpEntry);
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

void SemanticAnalyzer::visit(CompoundStatementNode *m) { //STATEMENT
    // Push Scope
    if( this->src_node.top() != FUNCTION_NODE) 
    {
        this->level_up();
        SymbolTable* new_scope = new SymbolTable(this->level);
        this->push(new_scope);
    }

    // Visit Child Nodes
    this->push_src_node(COMPOUND_STATEMENT_NODE);
        if (m->declaration_node_list != nullptr)
            for(uint i=0; i< m->declaration_node_list->size(); i++){
                (*(m->declaration_node_list))[i]->accept(*this);
            }
        
        if (m->statement_node_list != nullptr)
            for(uint i=0; i< m->statement_node_list->size(); i++){
                (*(m->statement_node_list))[i]->accept(*this);
            }
    this->pop_src_node();
    
    // Pop Scope
    if( this->src_node.top() != FUNCTION_NODE) 
    {
        this->pop();
        this->level_down();
    }
}

void SemanticAnalyzer::visit(AssignmentNode *m) { //STATEMENT
    
}

void SemanticAnalyzer::visit(PrintNode *m) { //STATEMENT
    //  Visit Child Node
    this->push_src_node(PRINT_NODE);
        if (m->expression_node != nullptr)
            m->expression_node->accept(*this);
    this->pop_src_node();

    // Semantic Check
    VariableInfo tmpInfo = this->expression_stack.top();
    this->expression_stack.pop();

    if(tmpInfo.type_set != SET_SCALAR){
        if(tmpInfo.type_set == UNKNOWN_SET && tmpInfo.type == UNKNOWN_TYPE) return;
        this->semantic_error = true;
        this->error_msg+=error_found_msg(m->expression_node->line_number, m->expression_node->col_number);
        this->error_msg+="variable reference of print statement must be scalar type\n";
        this->error_msg+=src_notation_msg(this->fp, m->expression_node->line_number, m->expression_node->col_number);
    }
}

void SemanticAnalyzer::visit(ReadNode *m) { //STATEMENT

}

void SemanticAnalyzer::visit(VariableReferenceNode *m) { //EXPRESSION
    // Semantic Check
    bool exist = true;
    bool m_error = false;
    if(check_symbol_inside(m->variable_name) == false){
        this->semantic_error = true;
        this->error_msg+=error_found_msg(m->line_number, m->col_number);
        this->error_msg+="use of undeclared identifier '"+m->variable_name+"'\n";
        this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
        exist = false;
        m_error = true;
    }

    if(m->expression_node_list != nullptr && m_error == false){
    // Part1: 
        // First visit expression list
        this->push_src_node(VARIABLE_REFERENCE_NODE);
            for(uint i=0; i< m->expression_node_list->size(); i++)
                    (*(m->expression_node_list))[i]->accept(*this);
        this->pop_src_node();

        // Check the expression stack
        int type_check = 0;
        for(uint i=0; i< m->expression_node_list->size(); i++){
            VariableInfo temp = this->expression_stack.top();
            expression_stack.pop();

            if(temp.type == UNKNOWN_TYPE && type_check !=0) 
                type_check = 2;
            else if(temp.type != TYPE_INTEGER && type_check == 0 ){
                type_check = 1;
                this->semantic_error = true;
                this->error_msg+=error_found_msg(m->expression_node_list->at(i)->line_number, m->expression_node_list->at(i)->col_number);
                this->error_msg+="index of array reference must be an integer\n";
                this->error_msg+=src_notation_msg(this->fp,m->expression_node_list->at(i)->line_number, m->expression_node_list->at(i)->col_number);
                m_error = true;
            }
        }

        // According type_check do error response
        switch(type_check){
            case 0:
                // No Problem
                break;
            case 1:
                // Error!
                break; 
            case 2:
                // Expression Node has Problem
                // No need to check
            default: 
                break;
        }

    // Part2:
        if(exist){
            unsigned int subscript_size = 
                this->get_symbol_entry(m->variable_name).variable_node->type->array_range.size();
            if(m->expression_node_list->size() > subscript_size){
                // Error!
                this->semantic_error = true;
                this->error_msg+=error_found_msg(m->line_number, m->col_number);
                this->error_msg+="there is an over array subscript\n";
                this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
                m_error = true;
            }
        }
    }

    // Put Expression Stack
    if(m_error == true) {
        // Error Happen in this node
        VariableInfo tmpInfo;
        tmpInfo.type_set = UNKNOWN_SET;
        tmpInfo.type = UNKNOWN_TYPE;

        this->expression_stack.push(tmpInfo);
    } else {
        if(m->expression_node_list != nullptr){
            VariableInfo EntryInfo = *this->get_symbol_entry(m->variable_name).variable_node->type;
            VariableInfo tmpInfo;
            tmpInfo.type_set = EntryInfo.type_set;
            tmpInfo.type = EntryInfo.type;

            unsigned int dimension = EntryInfo.array_range.size() - m->expression_node_list->size();
            if(dimension == 0){
                tmpInfo.type_set = SET_SCALAR;
            } else {
                for(uint i=0; i<dimension; i++)
                    tmpInfo.array_range.push_back(EntryInfo.array_range[i]);
            }
        
            this->expression_stack.push(tmpInfo);               
        } else {
            VariableInfo tmpInfo = *this->get_symbol_entry(m->variable_name).variable_node->type;
            this->expression_stack.push(tmpInfo);                           
        }
    }
}

void SemanticAnalyzer::visit(BinaryOperatorNode *m) { //EXPRESSION
    // Visit Child Node
    this->push_src_node(BINARY_OPERATOR_NODE);
        if (m->left_operand != nullptr)
                m->left_operand->accept(*this);

        if (m->right_operand != nullptr)
            m->right_operand->accept(*this);
    this->pop_src_node();

    //Semantic Check // Expression Stack
    VariableInfo rhs = this->expression_stack.top();
    this->expression_stack.pop();
    VariableInfo lhs = this->expression_stack.top();
    this->expression_stack.pop();
    bool error = false;

    VariableInfo tmpInfo;
    if(fault_type_check(lhs)&&fault_type_check(rhs)){
        switch(m->op){
            case OP_OR: 
            case OP_AND:
            //case OP_NOT:
                if((lhs.type_set == SET_SCALAR || lhs.type_set == SET_CONSTANT_LITERAL) &&
                    (lhs.type == TYPE_BOOLEAN) ) {;}
                else { error = true; }
                if((rhs.type_set == SET_SCALAR || rhs.type_set == SET_CONSTANT_LITERAL) &&
                    (rhs.type == TYPE_BOOLEAN) ) {;}
                else { error = true; }
                if(error == true){
                    this->semantic_error = true;
                    this->error_msg+=error_found_msg(m->line_number, m->col_number);
                    this->error_msg+="invalid operands to binary operation";
                    this->error_msg+=" '"+op_convert(m->op)+"' ";
                    this->error_msg+="('"+info_convert(lhs)+"' and '"+info_convert(rhs)+"')\n";
                    this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
                    break;
                }
                tmpInfo.type_set = SET_SCALAR;
                tmpInfo.type = TYPE_BOOLEAN;
                this->expression_stack.push(tmpInfo);
                break;
            case OP_LESS: 
            case OP_LESS_OR_EQUAL: 
            case OP_EQUAL: 
            case OP_GREATER:
            case OP_GREATER_OR_EQUAL:
            case OP_NOT_EQUAL: 
                if((lhs.type_set == SET_SCALAR || lhs.type_set == SET_CONSTANT_LITERAL) &&
                    (lhs.type == TYPE_INTEGER || lhs.type == TYPE_REAL) ) {;}
                else { error = true; }
                if((rhs.type_set == SET_SCALAR || rhs.type_set == SET_CONSTANT_LITERAL) &&
                    (rhs.type == TYPE_INTEGER || rhs.type == TYPE_REAL) ) {;}
                else { error = true; }
                
                if(error == true){
                    this->semantic_error = true;
                    this->error_msg+=error_found_msg(m->line_number, m->col_number);
                    this->error_msg+="invalid operands to binary operation";
                    this->error_msg+=" '"+op_convert(m->op)+"' ";
                    this->error_msg+="('"+info_convert(lhs)+"' and '"+info_convert(rhs)+"')\n";
                    this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
                    break;
                }

                tmpInfo.type_set = SET_SCALAR;
                tmpInfo.type = TYPE_BOOLEAN;
                this->expression_stack.push(tmpInfo);
                break;
            case OP_PLUS: // Special Case
                if( (lhs.type_set == SET_SCALAR || lhs.type_set == SET_CONSTANT_LITERAL) &&
                    (lhs.type == TYPE_STRING) &&
                    (rhs.type_set == SET_SCALAR || rhs.type_set == SET_CONSTANT_LITERAL) &&
                    (rhs.type == TYPE_STRING) ){
                    tmpInfo.type_set = SET_SCALAR;
                    tmpInfo.type = TYPE_STRING;
                    this->expression_stack.push(tmpInfo);
                    break;
                }
                // Forward Check
            case OP_MINUS: 
            case OP_MULTIPLY: 
            case OP_DIVIDE:
                if( (lhs.type_set == SET_SCALAR || lhs.type_set == SET_CONSTANT_LITERAL) &&
                    (lhs.type == TYPE_INTEGER) &&
                    (rhs.type_set == SET_SCALAR || rhs.type_set == SET_CONSTANT_LITERAL) &&
                    (rhs.type == TYPE_INTEGER) ){
                    tmpInfo.type_set = SET_SCALAR;
                    tmpInfo.type = TYPE_INTEGER;
                    this->expression_stack.push(tmpInfo);
                    break;
                }
                if( (lhs.type_set == SET_SCALAR || lhs.type_set == SET_CONSTANT_LITERAL) &&
                    (lhs.type == TYPE_INTEGER || lhs.type == TYPE_REAL) &&
                    (rhs.type_set == SET_SCALAR || rhs.type_set == SET_CONSTANT_LITERAL) &&
                    (rhs.type == TYPE_INTEGER || rhs.type == TYPE_REAL) ){
                    tmpInfo.type_set = SET_SCALAR;
                    tmpInfo.type = TYPE_REAL;
                    this->expression_stack.push(tmpInfo);
                    break;
                } else {
                    error = true;
                    this->semantic_error = true;
                    this->error_msg+=error_found_msg(m->line_number, m->col_number);
                    this->error_msg+="invalid operands to binary operation";
                    this->error_msg+=" '"+op_convert(m->op)+"' ";
                    this->error_msg+="('"+info_convert(lhs)+"' and '"+info_convert(rhs)+"')\n";
                    this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
                    break;
                }
                break;
            case OP_MOD: 
                if((lhs.type_set == SET_SCALAR || lhs.type_set == SET_CONSTANT_LITERAL) &&
                    (lhs.type == TYPE_INTEGER) ) {;}
                else { error = true; }
                if((rhs.type_set == SET_SCALAR || rhs.type_set == SET_CONSTANT_LITERAL) &&
                    (rhs.type == TYPE_INTEGER) ) {;}
                else { error = true; }
                
                if(error == true){
                    this->semantic_error = true;
                    this->error_msg+=error_found_msg(m->line_number, m->col_number);
                    this->error_msg+="invalid operands to binary operation";
                    this->error_msg+=" '"+op_convert(m->op)+"' ";
                    this->error_msg+="('"+info_convert(lhs)+"' and '"+info_convert(rhs)+"')\n";
                    this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
                    break;
                }

                tmpInfo.type_set = SET_SCALAR;
                tmpInfo.type = TYPE_INTEGER;
                this->expression_stack.push(tmpInfo);
                break;
            default: break;
        }
    } else {
        error = true;
    }

    if(error == true){
        // Error Has Happened Before or Now
        VariableInfo tmpInfo;
        tmpInfo.type_set = UNKNOWN_SET;
        tmpInfo.type = UNKNOWN_TYPE;
        this->expression_stack.push(tmpInfo);
    } else {
        ;
    }
}

void SemanticAnalyzer::visit(UnaryOperatorNode *m) { //EXPRESSION
    // Visit Child Node
    this->push_src_node(UNARY_OPERATOR_NODE);
        if (m->operand != nullptr)
                m->operand->accept(*this);
    this->pop_src_node();

    //Semantic Check // Expression Stack
    VariableInfo lhs = this->expression_stack.top();
    this->expression_stack.pop();
    bool error = false;

    VariableInfo tmpInfo;
    if(fault_type_check(lhs)){
        switch(m->op){
            case OP_NOT:
                if((lhs.type_set == SET_SCALAR || lhs.type_set == SET_CONSTANT_LITERAL) &&
                    (lhs.type == TYPE_BOOLEAN) ) {;}
                else { error = true; }
                if(error == true){
                    this->semantic_error = true;
                    this->error_msg+=error_found_msg(m->line_number, m->col_number);
                    this->error_msg+="invalid operand to unary operation";
                    this->error_msg+=" '"+op_convert(m->op)+"' ";
                    this->error_msg+="('"+info_convert(lhs)+"')\n";
                    this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
                    break;
                }
                tmpInfo.type_set = SET_SCALAR;
                tmpInfo.type = TYPE_BOOLEAN;
                this->expression_stack.push(tmpInfo);
                break;
            
            case OP_MINUS: 
                if((lhs.type_set == SET_SCALAR || lhs.type_set == SET_CONSTANT_LITERAL) &&
                    (lhs.type == TYPE_INTEGER || lhs.type == TYPE_REAL) ) {
                    tmpInfo.type_set = SET_SCALAR;
                    tmpInfo.type = lhs.type;
                    this->expression_stack.push(tmpInfo);
                    break;
                }
                else {
                    error = true; 
                    this->semantic_error = true;
                    this->error_msg+=error_found_msg(m->line_number, m->col_number);
                    this->error_msg+="invalid operands to unary operation";
                    this->error_msg+=" '"+op_convert(m->op)+"' ";
                    this->error_msg+="('"+info_convert(lhs)+"')\n";
                    this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
                    break;
                }
                break;
            default: break;
        }
    } else {
        error = true;
    }

    if(error == true){
        // Error Has Happened Before or Now
        VariableInfo tmpInfo;
        tmpInfo.type_set = UNKNOWN_SET;
        tmpInfo.type = UNKNOWN_TYPE;
        this->expression_stack.push(tmpInfo);
    } else {
        ;
    }
}

void SemanticAnalyzer::visit(IfNode *m) { //STATEMENT
    // Visit Child Nodes
    this->push_src_node(IF_NODE);
        if (m->condition != nullptr)
            m->condition->accept(*this);

        if (m->body != nullptr)
            for(uint i=0; i< m->body->size(); i++)
                (*(m->body))[i]->accept(*this);
        
        if (m->body_of_else != nullptr)
            for(uint i=0; i< m->body_of_else->size(); i++)
                (*(m->body_of_else))[i]->accept(*this);
    this->pop_src_node();

    // Semantic Check
    VariableInfo tmpInfo = this->expression_stack.top();
    this->expression_stack.pop();

    if(tmpInfo.type != TYPE_BOOLEAN){
        if(tmpInfo.type_set == UNKNOWN_SET && tmpInfo.type == UNKNOWN_TYPE) return;
        this->semantic_error = true;
        this->error_msg+=error_found_msg(m->condition->line_number, m->condition->col_number);
        this->error_msg+="the expression of condition must be boolean type\n";
        this->error_msg+=src_notation_msg(this->fp, m->condition->line_number, m->condition->col_number);
    }
}

void SemanticAnalyzer::visit(WhileNode *m) { //STATEMENT
    // Visit Child Nodes
    this->push_src_node(WHILE_NODE);
        if (m->condition != nullptr)
            m->condition->accept(*this);

        if (m->body != nullptr)
            for(uint i=0; i< m->body->size(); i++)
                (*(m->body))[i]->accept(*this);
    this->pop_src_node();

    // Semantic Check
    VariableInfo tmpInfo = this->expression_stack.top();
    this->expression_stack.pop();

    if(tmpInfo.type != TYPE_BOOLEAN){
        if(tmpInfo.type_set == UNKNOWN_SET && tmpInfo.type == UNKNOWN_TYPE) return;
        this->semantic_error = true;
        this->error_msg+=error_found_msg(m->condition->line_number, m->condition->col_number);
        this->error_msg+="the expression of condition must be boolean type\n";
        this->error_msg+=src_notation_msg(this->fp, m->condition->line_number, m->condition->col_number);
    }
}

void SemanticAnalyzer::visit(ForNode *m) { //STATEMENT
    // Push Scope
    this->level_up();
    SymbolTable* new_scope = new SymbolTable(this->level);
    this->push(new_scope);

    // Visit Child Node
    this->push_src_node(FOR_NODE);
        this->specify_on(KIND_LOOP_VAR);
        if (m->loop_variable_declaration != nullptr)
            m->loop_variable_declaration->accept(*this);
        this->specify_off();
        
        if (m->initial_statement != nullptr)
            m->initial_statement->accept(*this);

        if (m->condition != nullptr)
            m->condition->accept(*this);

        if (m->body != nullptr)
            for(uint i=0; i< m->body->size(); i++)
                (*(m->body))[i]->accept(*this);
    this->pop_src_node();

    // Semantic Check
    if(m->lower_bound > m->upper_bound){
        this->semantic_error = true;
        this->error_msg+=error_found_msg(m->line_number, m->col_number);
        this->error_msg+="the lower bound of iteration count must be smaller than or equal to the upper bound\n";
        this->error_msg+=src_notation_msg(this->fp, m->line_number, m->col_number);
    }

    // Pop Scope
    this->pop();
    this->level_down();
}

void SemanticAnalyzer::visit(ReturnNode *m) { //STATEMENT

}

void SemanticAnalyzer::visit(FunctionCallNode *m) { //EXPRESSION //STATEMENT

}
