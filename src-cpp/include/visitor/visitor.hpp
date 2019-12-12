#pragma once
#include "AST/ast.hpp"


// Forward declaration
class ProgramNode;
class DeclarationNode;
class VariableNode;
class ConstantValueNode;
class FunctionNode;
class CompoundStatementNode;
class AssignmentNode;
class PrintNode;
class ReadNode;
class VariableReferenceNode;
class BinaryOperatorNode;
class UnaryOperatorNode;
class IfNode;
class WhileNode;
class ForNode;
class ReturnNode;
class FunctionCallNode;

class ASTVisitorBase
{
    public:
        virtual void visit(ProgramNode *node) = 0;
        virtual void visit(DeclarationNode *node) = 0;
        virtual void visit(VariableNode *node) = 0;
        virtual void visit(ConstantValueNode *node) = 0;
        virtual void visit(FunctionNode *node) = 0;
        virtual void visit(CompoundStatementNode *node) = 0;
        virtual void visit(AssignmentNode *node) = 0;
        virtual void visit(PrintNode *node) = 0;
        virtual void visit(ReadNode *node) = 0;
        virtual void visit(VariableReferenceNode *node) = 0;
        virtual void visit(BinaryOperatorNode *node) = 0;
        virtual void visit(UnaryOperatorNode *node) = 0;
        virtual void visit(IfNode *node) = 0;
        virtual void visit(WhileNode *node) = 0;
        virtual void visit(ForNode *node) = 0;
        virtual void visit(ReturnNode *node) = 0;
        virtual void visit(FunctionCallNode *node) = 0;

        uint space_counter = 0;
        void space_counter_increase();
        void space_counter_decrease();
        void print_space();
};
