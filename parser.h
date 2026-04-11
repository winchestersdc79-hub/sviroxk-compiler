#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <memory>

// Типы узлов дерева
enum NodeType {
    NODE_VAR_DECL,    // объявление переменной
    NODE_NUMBER,      // число
    NODE_STRING,      // текст
    NODE_IDENTIFIER,  // имя переменной
    NODE_SLOV,        // вывод текста
};

// Узел дерева
struct Node {
    NodeType type;
    std::string value;
    std::string varType;  // тип переменной (rox, dor...)
    std::string varName;  // имя переменной
};

// Функция парсера
Node parse(const std::vector<Token>& tokens);

#endif
