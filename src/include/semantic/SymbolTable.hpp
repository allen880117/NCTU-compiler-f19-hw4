#pragma once
#include <AST/ast.hpp>
#include <vector>
#include <map>
#include <string>
using namespace std;

enum AttributeType{
    NO_ATTRIBUTE = 200,
    ATTRIBUTE_PARAMETERS,
    ATTRIBUTE_VALUE_OF_CONSTANT,
    UNKNOWN_ATTRIBUTE,
};

class Attribute{
    public:
        vector<VariableInfo> parameter_type;
        VariableInfo         value_of_constant;
        AttributeType        attr_type;
    
    public:
        Attribute();
        Attribute(AttributeType);

        void set_parameter_type(vector<VariableInfo>);
        void set_value_of_constant(VariableInfo);
};

enum FieldKind{
    KIND_PROGRAM = 100,
    KIND_FUNCTION,
    KIND_PARAMETER,
    KIND_VARIABLE,
    KIND_LOOP_VAR,
    KIND_CONSTANT,
    KIND_UNKNOWN
};

class SymbolEntry{
    public:
        // Public Info
        string       name; // size = 1 to 32
        FieldKind    kind;
        unsigned int level;
        VariableInfo type;
        Attribute    attribute;

        // Hide Info
        bool         is_used;
        enum NODE_TABLE node_type;
        
        class ProgramNode* program_node;
        //class DeclarationNode* declaration_node;
        class VariableNode* variable_node;
        //class ConstantValueNode* constant_value_node;
        class FunctionNode* function_node; 
        //class CompoundStatementNode* compound_statement_node;
        //class AssignmentNode* assignment_node;
        //class PrintNode* print_node;
        //class ReadNode* read_node;
        //class VariableReferenceNode* variable_reference_node;
        //class BinaryOperatorNode* binary_operator_node;
        //class UnaryOperatorNode* unary_operator_node;
        //class IfNode* if_node;
        //class WhileNode* while_node;
        //class ForNode* for_node;
        //class ReturnNode* return_node;
        //class FunctionCallNode* function_call_node;

    public:
        SymbolEntry();
        SymbolEntry(
            string _name,
            FieldKind _kind,
            unsigned int _level,
            VariableInfo _type,
            Attribute _attribute,
            enum NODE_TABLE _node_type);
};

class SymbolTable{
    public:
        // Link Info
        SymbolTable* prev_scope;
        vector<SymbolTable*> next_scope_list;
        
        // General Info
        unsigned int             level;
        map<string, SymbolEntry> entry;
        vector<string>           entry_name;

    public:
        SymbolTable(unsigned int _level);
        ~SymbolTable();

        void put(SymbolEntry _symbol_entry);
        bool redeclare_check(string _name);
};