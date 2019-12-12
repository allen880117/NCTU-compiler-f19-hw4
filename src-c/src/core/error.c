#include "core/error.h"
#include "AST/ast.h"

#include <stdarg.h>
#include <stdio.h>

static const uint32_t kIndentionWidth = 4;

void logSemanticError(const Location *location, const char *format, ...) {
    fprintf(stderr, "<Error> Found in line %u, column %u: ", location->line,
            location->col);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    // TODO: get the source code and output it with notation '^'
}
