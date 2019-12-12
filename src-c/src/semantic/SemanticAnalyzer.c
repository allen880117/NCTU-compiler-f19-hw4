#include "semantic/SemanticAnalyzer.h"
#include "core/error.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* ============================
 * > Helper Function
 * ============================ */
static inline void constructSymbolTable(/* object that maintains the symbol tables */) {
    // TODO
}

static inline void destructSymbolTable(/* object that maintains the symbol tables */) {
    // TODO
}

/* ============================
 * > Program Node
 * ============================ */
static inline void addSymbolFromProgramNode(/* ... */) {
    // See if symbol is redeclared
    if (/* addSymbol() */ false == false) {
        // logSemanticError(TODO);
    }
}

static inline void semanticCheckForProgramNode(/* ... */) {
    // See if the program name is the same as filename
    if (/* strcmp(xxx, yyy) == 0 */ 0 == 0) {
        // logSemanticError(TODO);
    }

    // See if the program name is consistent
    if (/* strcmp(xxx, yyy) == 0 */ 0 == 0) {
        // logSemanticError(TODO);
    }
}

static void analyzeProgramNode(AstNodeVisitor *visitor, ProgramNode *node) {

    constructSymbolTable(/* ... */);
    addSymbolFromProgramNode(/* ... */);

    // TODO: visit child nodes of ProgramNode
    semanticCheckForProgramNode(/* ... */);

    destructSymbolTable(/* ... */);
}

/* ============================
 * > Declaration Node
 * ============================ */
static void analyzeDeclarationNode(AstNodeVisitor *visitor,
                                   DeclarationNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > Variable Node
 * ============================ */
static void analyzeVariableNode(AstNodeVisitor *visitor, VariableNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > Constant Value Node
 * ============================ */
static void analyzeConstantValueNode(AstNodeVisitor *visitor,
                                     ConstantValueNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > Function Node
 * ============================ */
static void analyzeFunctionNode(AstNodeVisitor *visitor, FunctionNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > Compound Statement Node
 * ============================ */
static void analyzeCompoundStatementNode(AstNodeVisitor *visitor,
                                         CompoundStatementNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > Assignment Node
 * ============================ */
static void analyzeAssignmentNode(AstNodeVisitor *visitor,
                                  AssignmentNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > Print Node
 * ============================ */
static void analyzePrintNode(AstNodeVisitor *visitor, PrintNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > Read Node
 * ============================ */
static void analyzeReadNode(AstNodeVisitor *visitor, ReadNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > Variable Reference Node
 * ============================ */
static void analyzeVariableReferenceNode(AstNodeVisitor *visitor,
                                         VariableReferenceNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > Binary Operator Node
 * ============================ */
static void analyzeBinaryOperatorNode(AstNodeVisitor *visitor,
                                      BinaryOperatorNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > Unary Operator Node
 * ============================ */
static void analyzeUnaryOperatorNode(AstNodeVisitor *visitor,
                                     UnaryOperatorNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > If Node
 * ============================ */
static void analyzeIfNode(AstNodeVisitor *visitor, IfNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > While Node
 * ============================ */
static void analyzeWhileNode(AstNodeVisitor *visitor, WhileNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > For Node
 * ============================ */
static void analyzeForNode(AstNodeVisitor *visitor, ForNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > Return Node
 * ============================ */
static void analyzeReturnNode(AstNodeVisitor *visitor, ReturnNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > Function Call Node
 * ============================ */
static void analyzeFunctionCallNode(AstNodeVisitor *visitor,
                                    FunctionCallNode *node) {
    /*
     * TODO:
     *     Push a new symbol table if needed.
     *     Insert a symbol into current symbol table if needed.
     *     Visit child nodes
     *     Perform semantic analyses
     *     Pop the symbol table if pushed
     */
}

/* ============================
 * > Semantic Analyze Visitor
 * ============================ */
AstNodeVisitor *newSemanticAnalyzer(/* ... */) {
    SemanticAnalyzer *visitor =
        (SemanticAnalyzer *)malloc(sizeof(SemanticAnalyzer));
    CONSTRUCT_VISITOR(visitor, analyze);
    // TODO: prepare some members

    return (AstNodeVisitor *)visitor;
}

void freeSemanticAnalyzer(SemanticAnalyzer *sa) {
    // TODO: other members
    free(sa);
}
