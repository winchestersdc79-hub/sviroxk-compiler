#ifndef SC_SEMANTIC_H
#define SC_SEMANTIC_H

#include "parser.h"
#include <string>

// Returns false and prints semantic/type diagnostics if the AST is invalid.
bool checkSemantics(const Node& program, const std::string& filename);

#endif
