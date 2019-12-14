/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 73 "parser.y" /* yacc.c:1909  */
 #include "AST/ast.hpp" 
#line 74 "parser.y" /* yacc.c:1909  */
 #include "AST/program.hpp" 
#line 75 "parser.y" /* yacc.c:1909  */
 #include "AST/declaration.hpp" 
#line 76 "parser.y" /* yacc.c:1909  */
 #include "AST/variable.hpp" 
#line 77 "parser.y" /* yacc.c:1909  */
 #include "AST/constant_value.hpp" 
#line 78 "parser.y" /* yacc.c:1909  */
 #include "AST/function.hpp" 
#line 79 "parser.y" /* yacc.c:1909  */
 #include "AST/compound_statement.hpp" 
#line 80 "parser.y" /* yacc.c:1909  */
 #include "AST/assignment.hpp" 
#line 81 "parser.y" /* yacc.c:1909  */
 #include "AST/print.hpp" 
#line 82 "parser.y" /* yacc.c:1909  */
 #include "AST/read.hpp" 
#line 83 "parser.y" /* yacc.c:1909  */
 #include "AST/variable_reference.hpp" 
#line 84 "parser.y" /* yacc.c:1909  */
 #include "AST/binary_operator.hpp" 
#line 85 "parser.y" /* yacc.c:1909  */
 #include "AST/unary_operator.hpp" 
#line 86 "parser.y" /* yacc.c:1909  */
 #include "AST/if.hpp" 
#line 87 "parser.y" /* yacc.c:1909  */
 #include "AST/while.hpp" 
#line 88 "parser.y" /* yacc.c:1909  */
 #include "AST/for.hpp" 
#line 89 "parser.y" /* yacc.c:1909  */
 #include "AST/return.hpp" 
#line 90 "parser.y" /* yacc.c:1909  */
 #include "AST/function_call.hpp" 
#line 91 "parser.y" /* yacc.c:1909  */
 #include "visitor/visitor.hpp" 
#line 92 "parser.y" /* yacc.c:1909  */
 #include "semantic/SemanticAnalyzer.hpp" 
#line 93 "parser.y" /* yacc.c:1909  */
 #include "semantic/SymbolTable.hpp" 
#line 94 "parser.y" /* yacc.c:1909  */
 #include "semantic/ErrorMsg.hpp" 
#line 95 "parser.y" /* yacc.c:1909  */
 #include "semantic/DumpSymbolTable.hpp" 

#line 91 "parser.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    COMMA = 258,
    SEMICOLON = 259,
    COLON = 260,
    L_PARENTHESIS = 261,
    R_PARENTHESIS = 262,
    L_BRACKET = 263,
    R_BRACKET = 264,
    ASSIGN = 265,
    OR = 266,
    AND = 267,
    NOT = 268,
    LESS = 269,
    LESS_OR_EQUAL = 270,
    EQUAL = 271,
    GREATER = 272,
    GREATER_OR_EQUAL = 273,
    NOT_EQUAL = 274,
    PLUS = 275,
    MINUS = 276,
    MULTIPLY = 277,
    DIVIDE = 278,
    MOD = 279,
    UNARY_MINUS = 280,
    ARRAY = 281,
    BOOLEAN = 282,
    INTEGER = 283,
    REAL = 284,
    STRING = 285,
    END = 286,
    BEGIN_ = 287,
    DO = 288,
    ELSE = 289,
    FOR = 290,
    IF = 291,
    THEN = 292,
    WHILE = 293,
    DEF = 294,
    OF = 295,
    TO = 296,
    RETURN = 297,
    VAR = 298,
    FALSE = 299,
    TRUE = 300,
    PRINT = 301,
    READ = 302,
    ID = 303,
    INT_LITERAL = 304,
    REAL_LITERAL = 305,
    STRING_LITERAL = 306
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 98 "parser.y" /* yacc.c:1909  */

    int    val;
    double dval;
    char*  text;

    enum enumOperator op_type;

    Node      node;
    NodeList* node_list_ptr;

    struct NodeWithTypeList*           node_w_type_list_ptr;
    vector<struct NodeWithTypeList*>*  node_w_type_list_ptr_list_ptr;

    vector<struct id_info>* id_list_ptr;
    VariableInfo*           variable_info_ptr;

#line 172 "parser.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_PARSER_H_INCLUDED  */
