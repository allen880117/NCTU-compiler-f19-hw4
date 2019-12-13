#pragma once
#include "visitor/visitor.hpp"
#include "semantic/SymbolTable.hpp"
#include <vector>
#include <string>

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
        
        SemanticAnalyzer();
        SemanticAnalyzer(string _filename, FILE* _fp, int _dump_enable);
        //~SemanticAnalyzer();

        class SymbolTable* getSymbolTable(); 
        int  semantic_error;
        void  output_err_msg();

    private: // TODO
        class SymbolTable* symbol_table_root;
        class SymbolTable* current_scope;
        unsigned int       level;
        
        std::string filename;
        FILE* fp;
        int  dump_enable;
        std::string error_msg;

        void  level_up();
        void  level_down();
};