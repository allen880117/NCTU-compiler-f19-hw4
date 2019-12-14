#pragma once
#include "visitor/visitor.hpp"
#include "semantic/SymbolTable.hpp"
#include <vector>
#include <string>
#include <cstdio>
using namespace std;
// FIXME: remember to replace ";" below with ";", or your implementations in .cpp won't be compiled.

class SemanticAnalyzer : public ASTVisitorBase
{
    public:
        void visit(ProgramNode *m) override ;
        void visit(DeclarationNode *m) override ;
        void visit(VariableNode *m) override ;
        void visit(ConstantValueNode *m) override ;
        void visit(FunctionNode *m) override ;
        void visit(CompoundStatementNode *m) override ;
        void visit(AssignmentNode *m) override ;
        void visit(PrintNode *m) override ;
        void visit(ReadNode *m) override ;
        void visit(VariableReferenceNode *m) override ;
        void visit(BinaryOperatorNode *m) override ;
        void visit(UnaryOperatorNode *m) override ;
        void visit(IfNode *m) override ;
        void visit(WhileNode *m) override ;
        void visit(ForNode *m) override ;
        void visit(ReturnNode *m) override ;
        void visit(FunctionCallNode *m) override ;
        
        SemanticAnalyzer(string _filename, FILE* _fp, int _dump_enable);
        ~SemanticAnalyzer(){}

        class SymbolTable* get_symbol_table(); 
        void               output_err_msg();
        int                is_semantic_error();

    private: // TODO
        class SymbolTable* symbol_table_root;
        class SymbolTable* current_scope;
        unsigned int       level;
        
        string filename;
        FILE*  fp;
        int    dump_enable;

        string error_msg;
        int    semantic_error;

        void  level_up();
        void  level_down();
        void  push(SymbolTable* _new_scope);
        void  pop();

        bool      specify;
        FieldKind specify_kind;
        void  specify_on(FieldKind);
        void  specify_off();

        bool  compound_level_up_need;
        void  compound_level_up_need_on();
        void  compound_level_up_need_off();
};