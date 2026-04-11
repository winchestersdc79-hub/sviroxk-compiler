#include <iostream>
#include "parser.h"
#include <stdexcept>

// Текущая позиция в списке токенов
static int pos = 0;

// Посмотреть текущий токен
Token peek(const std::vector<Token>& tokens) {
    return tokens[pos];
}

// Взять текущий токен и перейти к следующему
Token consume(const std::vector<Token>& tokens) {
    return tokens[pos++];
}

// Ожидать конкретный токен
Token expect(const std::vector<Token>& tokens, TokenType type) {
    Token t = consume(tokens);
    if (t.type != type) {
        std::cerr << "Ошибка: неожиданный токен: " << t.value << std::endl;
        exit(1);
    }
    return t;
}

// Парсим объявление переменной
// svi rox lor x = 5;
Node parseVarDecl(const std::vector<Token>& tokens) {
    Node node;
    node.type = NODE_VAR_DECL;

    consume(tokens);  // svi

    // тип переменной (rox, dor, cos, bue)
    Token typeToken = consume(tokens);
    node.varType = typeToken.value;

    consume(tokens);  // lor

    // имя переменной
    Token nameToken = consume(tokens);
    node.varName = nameToken.value;

    expect(tokens, EQUALS);  // =

    // значение
    Token valueToken = consume(tokens);
    node.value = valueToken.value;

    expect(tokens, SEMICOLON);  // ;

    return node;
}

// Главная функция парсера
Node parse(const std::vector<Token>& tokens) {
    pos = 0;

    if (peek(tokens).type == SVI) {
        return parseVarDecl(tokens);
    }

    std::cerr << "Неизвестная конструкция" << std::endl;
    exit(1);
}
