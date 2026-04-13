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
    NODE_FUNC_DEF,
    NODE_FUNC_CALL,
    NODE_STRUCT_DEF,
    NODE_ARG,
    NODE_RETURN,
    NODE_ARRAY_DECL,
    NODE_ARRAY_ACCESS,
    NODE_ADDR,
    NODE_DEREF,
    NODE_DEREF_ASSIGN,
    NODE_ARRAY_ASSIGN,
    NODE_DYN_ARRAY,
    NODE_DELETE,
    NODE_BREAK,
    NODE_CONTINUE,
    NODE_FILE_OPEN,
    NODE_INPUT,
    NODE_STRLEN,
    NODE_STRCAT,
    NODE_FILE_WRITE,
    NODE_FILE_READ,
    NODE_FILE_CLOSE,
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
    Node* extra = nullptr;
    std::vector<Node> children;
    std::vector<Node> elseChildren;
    std::vector<Node> args;
    std::vector<std::string> params;
};

Node parse(const std::vector<Token>& tokens);

#endif
