#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <memory>
#include <vector>

enum NodeType {
    NODE_VAR_DECL,
    NODE_NUMBER,
    NODE_STRING,
    NODE_IDENTIFIER,
    NODE_SLOV,
    NODE_BINOP,    // математическая операция
};

struct Node {
    NodeType type;
    std::string value;
    std::string varType;
    std::string varName;
    std::string op;        // +, -, *, /
    Node* left = nullptr;  // левая часть
    Node* right = nullptr; // правая часть
};

Node parse(const std::vector<Token>& tokens);

#endif
