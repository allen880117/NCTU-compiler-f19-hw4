#ifndef __SEMANTIC_ANALYZER_H
#define __SEMANTIC_ANALYZER_H

#include "visitor/visitor.h"

typedef struct __SemanticAnalyzer {
    // TODO
} SemanticAnalyzer;

AstNodeVisitor *newSemanticAnalyzer(/* ... */);
void freeSemanticAnalyzer(SemanticAnalyzer *sa);

#endif
