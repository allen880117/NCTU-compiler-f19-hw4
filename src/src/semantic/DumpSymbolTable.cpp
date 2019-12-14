#include "semantic/DumpSymbolTable.hpp"
#include <iostream>
#include <iomanip>
#include <cstdio>
using namespace std;

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