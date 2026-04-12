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
    NODE_PROGRAM,
    NODE_IF,
    NODE_LOOP_N,
    NODE_ASSIGN,
    NODE_LOOP_W,
};

struct Node {
    NodeType type;
    std::string value;
    std::string varType;
    std::string varName;
    std::string op;
    Node* left = nullptr;
    Node* right = nullptr;
    std::vector<Node> children;
    std::vector<Node> elseChildren;
};

Node parse(const std::vector<Token>& tokens);

#endif
