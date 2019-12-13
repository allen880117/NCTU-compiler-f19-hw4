#include "semantic/SemanticAnalyzer.hpp"
#include "semantic/SymbolTable.hpp"
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

SemanticAnalyzer::SemanticAnalyzer(){
    this->symbol_table_root = nullptr;
    this->current_scope     = nullptr;
    this->level             = 0;
    this->semantic_error = 0;
    this->error_msg = "";
}

SemanticAnalyzer::SemanticAnalyzer(string _filename, FILE* _fp, int _dump_enable){
    this->symbol_table_root = nullptr;
    this->current_scope     = nullptr;
    this->level             = 0;
    _filename = _filename.substr(0, _filename.length()-2);
    /*
    for(uint i=_filename.length()-1; i>=0; i++){
        if(_filename[i] == '/' ){
            _filename = _filename.substr(i+1, _filename.length()-i);
            break;
        }
    }
    */
    this->filename = _filename;
   
    this->fp = _fp;
    this->dump_enable = _dump_enable;

    this->semantic_error = 0;
    this->error_msg = "";
}

void SemanticAnalyzer::level_up(){this->level++;}
void SemanticAnalyzer::level_down(){this->level--;}
SymbolTable* SemanticAnalyzer::getSymbolTable(){return this->symbol_table_root;}
void SemanticAnalyzer::output_err_msg(){cout<<this->error_msg;}
//
// TODO: Dump Function
//

string info_convert(VariableInfo input){
    string msg = "";
    switch(input.type_set){
        case SET_SCALAR:
        case SET_CONSTANT_LITERAL:
            switch(input.type){
                case TYPE_INTEGER: msg = "integer"; break;
                case TYPE_REAL:    msg = "real";    break;
                case TYPE_STRING:  msg = "string";  break;
                case TYPE_BOOLEAN: msg = "boolean"; break;
                default:           msg = "unknown"; break;
            }
            break;
        case SET_ACCUMLATED:
            switch(input.type){
                case TYPE_INTEGER: msg = "integer"; break;
                case TYPE_REAL:    msg = "real";    break;
                case TYPE_STRING:  msg = "string";  break;
                case TYPE_BOOLEAN: msg = "boolean"; break;
                default:           msg = "unknown"; break;
            }
            msg += " ";
            for(uint i=0; i<input.array_range.size(); i++){
                msg += "[";
                msg += to_string(input.array_range[i].end-input.array_range[i].start);
                msg += "]";
            }
            break;
        case UNKNOWN_SET:
            switch(input.type){
                case TYPE_VOID: msg = "void"; break;
                default:        msg = "unknown"; break;
            }
            break;
        default: msg = "unknown"; break;
    }
    return msg;
}

void dumpDemarcation(const char chr) {
  for (size_t i = 0; i < 110; ++i) {
    cout << chr;
    //printf("%c", chr);
  }
  cout << endl;
  //puts("");
}
void dumpSymbol_Header(){
    dumpDemarcation('=');
    printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type", "Attribute");
    dumpDemarcation('-');
}

void dumpSymbol_Body(SymbolEntry symbol_entry) {
    //printf("%-33s", "func");
    cout << std::right << std::left << setw(33) << symbol_entry.name;
    
    //printf("%-11s", "function");
    switch(symbol_entry.kind){
        case KIND_PROGRAM:   cout << std::right << std::left << setw(11) << "program";   break;
        case KIND_FUNCTION:  cout << std::right << std::left << setw(11) << "function";  break;
        case KIND_PARAMETER: cout << std::right << std::left << setw(11) << "parameter"; break;
        case KIND_VARIABLE:  cout << std::right << std::left << setw(11) << "variable";  break;
        case KIND_LOOP_VAR:  cout << std::right << std::left << setw(11) << "loop_var";  break;
        case KIND_CONSTANT:  cout << std::right << std::left << setw(11) << "constant";  break;
        default: cout << std::right << std::left << setw(33) << "unknown"; break;
    }

    //printf("%d%-10s", 0, "(global)");
    switch(symbol_entry.level){
        case 0:  cout << std::right << 0 << std::left << setw(10) << "(global)";                 break;
        default: cout << std::right << symbol_entry.level << std::left << setw(10) << "(local)"; break;
    }

    //printf("%-17s", "boolean");
    cout << std::right << std::left << setw(17) << info_convert(symbol_entry.type);

    //printf("%-11s", "integer, real [2][3]");
    string msg = "";
    switch(symbol_entry.attribute.attr_type){
        case ATTRIBUTE_PARAMETERS: 
            for(uint i=0; i<symbol_entry.attribute.parameter_type.size(); i++){
                if(i != 0) msg+=", ";
                msg += info_convert(symbol_entry.attribute.parameter_type[i]);
            }
            cout << std::right << std::left << setw(11) << msg;
            break;
        case ATTRIBUTE_VALUE_OF_CONSTANT:
            switch(symbol_entry.attribute.value_of_constant.type){
                case TYPE_INTEGER: std::cout << std::right << std::left << setw(11) << symbol_entry.attribute.value_of_constant.int_literal; break;
                case TYPE_REAL:    std::cout << std::right << std::left << setw(11) << fixed << setprecision(6) << symbol_entry.attribute.value_of_constant.real_literal; break;
                case TYPE_STRING:  std::cout << std::right << std::left << setw(11) << symbol_entry.attribute.value_of_constant.string_literal; break;
                case TYPE_BOOLEAN:
                    switch(symbol_entry.attribute.value_of_constant.boolean_literal){
                        case Boolean_TRUE:  std::cout << std::right << std::left << setw(11) << "true"; break;
                        case Boolean_FALSE: std::cout << std::right << std::left << setw(11) << "false"; break;
                        default: std::cout << std::right << std::left << setw(11) << "unknown"; break;
                    } 
                    break;
                default: std::cout << std::right << std::left << setw(11) << "unknown"; break;
            }
            break;
        case NO_ATTRIBUTE:
        case UNKNOWN_ATTRIBUTE: 
        default: break;
    }

    //puts("");
    cout << std::right << endl;
}

void dumpSymbol_Bottom(){
  dumpDemarcation('-');
}

// 
// Error Message
//

string src_notation_msg(FILE* fp, uint32_t line_num, uint32_t col_num){
    string temp="";
    char buffer[1024];
    fseek( fp, 0, SEEK_SET ); // Back to Start
    for(uint i=0; i<line_num; i++){
        fgets(buffer, 1024, fp);
    }

    temp+="    ";
    temp+=string(buffer);

    temp+="    ";
    for(uint i=0; i<col_num-1; i++) temp+=" ";

    temp+="^\n";
    return temp;
}

string redeclare_error_msg(uint32_t x, uint32_t y, string symbol_name){
    string temp="";
    temp+="<Error> Found in line ";
    temp+=to_string(x);
    temp+=", column ";
    temp+=to_string(y);
    temp+=": symbol '";
    temp+=symbol_name;
    temp+="' is redeclared";
    temp+="\n";
    return temp;
}

string error_found_msg(uint32_t x, uint32_t y){
    string temp="";
    temp+="<Error> Found in line ";
    temp+=to_string(x);
    temp+=", column ";
    temp+=to_string(y);
    temp+=": ";
    return temp;
}
//
// TODO: implementations of visit(xxxxNode *)
//

void SemanticAnalyzer::visit(ProgramNode *m) {
    // Initialize Symbol Table
    this->symbol_table_root = new SymbolTable();
    this->current_scope = this->symbol_table_root;

    // Push Symbol Entity
    if(current_scope->redeclare_check(m->program_name) == false ){
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
    this->level_up();
        if (m->declaration_node_list != nullptr)
            for(uint i=0; i< m->declaration_node_list->size(); i++){
                (*(m->declaration_node_list))[i]->accept(*this);
            }
    this->level_down();
    this->level_up();
        if (m->function_node_list != nullptr)
            for(uint i=0; i< m->function_node_list->size(); i++){
                (*(m->function_node_list))[i]->accept(*this);
            }
    this->level_down();
    this->level_up();
        if (m->compound_statement_node != nullptr)
            m->compound_statement_node->accept(*this);
    this->level_down();

    // Output Symbol
    if (dump_enable == 1){
        dumpSymbol_Header();
        for(uint i=0; i<this->current_scope->entry_name.size(); i++)
            dumpSymbol_Body(this->current_scope->entry[this->current_scope->entry_name[i]]);
        dumpSymbol_Bottom();
    }

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

}

void SemanticAnalyzer::visit(DeclarationNode *m) {}

void SemanticAnalyzer::visit(VariableNode *m) {}

void SemanticAnalyzer::visit(ConstantValueNode *m) {}

void SemanticAnalyzer::visit(FunctionNode *m) {}

void SemanticAnalyzer::visit(CompoundStatementNode *m) {}

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
