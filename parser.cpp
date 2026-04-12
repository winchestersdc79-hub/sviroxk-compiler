#include "parser.h"
#include <iostream>

static int pos = 0;

Token peek(const std::vector<Token>& tokens) {
    return tokens[pos];
}

Token consume(const std::vector<Token>& tokens) {
    return tokens[pos++];
}

Token expect(const std::vector<Token>& tokens, TokenType type) {
    Token t = consume(tokens);
    if (t.type != type) {
        std::cerr << "Ошибка: неожиданный токен: " << t.value << std::endl;
        exit(1);
    }
    return t;
}

// Парсим одно значение или выражение
Node parseExpr(const std::vector<Token>& tokens) {
    Node left;
    Token t = consume(tokens);

    if (t.type == NUMBER) {
        left.type = NODE_NUMBER;
        left.value = t.value;
    } else if (t.type == IDENTIFIER) {
        left.type = NODE_IDENTIFIER;
        left.value = t.value;
    } else if (t.type == STRING) {
        left.type = NODE_STRING;
        left.value = t.value;
    }

    // Проверяем есть ли оператор
    Token next = peek(tokens);
    if (next.value == "+" || next.value == "-" ||
        next.value == "*" || next.value == "/") {
        consume(tokens); // оператор
        Node* binop = new Node();
        binop->type = NODE_BINOP;
        binop->op = next.value;
        binop->left = new Node(left);
        binop->right = new Node(parseExpr(tokens));
        return *binop;
    }

    return left;
}

Node parseVarDecl(const std::vector<Token>& tokens) {
    Node node;
    node.type = NODE_VAR_DECL;
    consume(tokens);  // svi
    Token typeToken = consume(tokens);
    node.varType = typeToken.value;
    consume(tokens);  // lor
    Token nameToken = consume(tokens);
    node.varName = nameToken.value;
    expect(tokens, EQUALS);
    node.left = new Node(parseExpr(tokens));
    expect(tokens, SEMICOLON);
    return node;
}

Node parse(const std::vector<Token>& tokens) {
    pos = 0;

    if (peek(tokens).type == SVI) {
        return parseVarDecl(tokens);
    }

    if (peek(tokens).type == SLOV) {
        Node node;
        node.type = NODE_SLOV;
        consume(tokens);
        expect(tokens, LPAREN);
        Token val = consume(tokens);
        node.value = val.value;
        expect(tokens, RPAREN);
        expect(tokens, SEMICOLON);
        return node;
    }

    std::cerr << "Неизвестная конструкция" << std::endl;
    exit(1);
}
