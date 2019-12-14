#include "semantic/SymbolTable.hpp"

Attribute::Attribute(){
    this->attr_type = UNKNOWN_ATTRIBUTE;
}

Attribute::Attribute(AttributeType _attr_type){
    this->attr_type = _attr_type;
}

void Attribute::set_parameter_type(vector<VariableInfo> _parameters){
    this->parameter_type = _parameters;
    this->attr_type = ATTRIBUTE_PARAMETERS;
}

void Attribute::set_value_of_constant(VariableInfo _value){
    this->value_of_constant = _value;
    this->attr_type = ATTRIBUTE_VALUE_OF_CONSTANT;
}

SymbolEntry::SymbolEntry(){
    this->kind = KIND_UNKNOWN;
    this->is_used = false;
}

SymbolEntry::SymbolEntry(
    string _name,
    FieldKind _kind,
    unsigned int _level,
    VariableInfo _type,
    Attribute _attribute
    ){
        if(_name.length() > 32) this->name = _name.substr(0, 32);
        else                    this->name = _name;
        this->kind = _kind;
        this->level = _level;
        this->type = _type;
        this->attribute = _attribute;

        this->is_used = true;
    }

SymbolTable::SymbolTable(unsigned int _level){
    this->prev_scope = NULL;
    this->next_scope_list.clear();
    this->level = _level;
    this->entry.clear();
    this->entry_name.clear();
}

SymbolTable::~SymbolTable(){
    SAFE_DELETE(this->prev_scope)

    for(uint i=0; i<this->next_scope_list.size(); i++){
        SAFE_DELETE(this->next_scope_list[i])
    }
}

void SymbolTable::put(SymbolEntry _symbol_entry){
    this->entry[_symbol_entry.name] = _symbol_entry;
    this->entry_name.push_back(_symbol_entry.name);
}

// Check the Variable is Redeclared Before
// false -> redeclare happen
bool SymbolTable::redeclare_check(string _name){
    if(_name.length()>32) _name = _name.substr(0, 32);
    if(this->entry[_name].is_used == true){ return false; }
    else { return true; }        
}