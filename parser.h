#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <vector>

enum NodeType {
    NODE_VAR_DECL,
    NODE_NUMBER,
    NODE_STRING,
    NODE_IDENTIFIER,
    NODE_SLOV,
    NODE_BINOP,
    NODE_PROGRAM,  // весь файл
};

struct Node {
    NodeType type;
    std::string value;
    std::string varType;
    std::string varName;
    std::string op;
    Node* left = nullptr;
    Node* right = nullptr;
    std::vector<Node> children;  // список инструкций
};

Node parse(const std::vector<Token>& tokens);

#endif
