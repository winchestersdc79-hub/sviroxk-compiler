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

Node parseExpr(const std::vector<Token>& tokens) {
    Node left;
    Token t = consume(tokens);
    if (t.type == NUMBER) { left.type = NODE_NUMBER; left.value = t.value; }
    else if (t.type == STRING) { left.type = NODE_STRING; left.value = t.value; }
    else if (t.type == IDENTIFIER) { left.type = NODE_IDENTIFIER; left.value = t.value; }

    Token next = peek(tokens);
    if (next.value == "+" || next.value == "-" ||
        next.value == "*" || next.value == "/") {
        consume(tokens);
        Node* binop = new Node();
        binop->type = NODE_BINOP;
        binop->op = next.value;
        binop->left = new Node(left);
        binop->right = new Node(parseExpr(tokens));
        return *binop;
    }
    return left;
}

Node parseCond(const std::vector<Token>& tokens) {
    Node left = parseExpr(tokens);
    Token op = consume(tokens);
    Node right = parseExpr(tokens);
    Node* cond = new Node();
    cond->type = NODE_BINOP;
    cond->op = op.value;
    cond->left = new Node(left);
    cond->right = new Node(right);
    return *cond;
}

std::vector<Node> parseBlock(const std::vector<Token>& tokens);

Node parseOne(const std::vector<Token>& tokens) {
    if (peek(tokens).type == SVI) {
        Node node;
        node.type = NODE_VAR_DECL;
        consume(tokens);
        Token typeToken = consume(tokens);
        node.varType = typeToken.value;
        consume(tokens);
        Token nameToken = consume(tokens);
        node.varName = nameToken.value;
        expect(tokens, EQUALS);
        node.left = new Node(parseExpr(tokens));
        expect(tokens, SEMICOLON);
        return node;
    }
    if (peek(tokens).type == SLOV) {
        Node node;
        node.type = NODE_SLOV;
        consume(tokens);
        expect(tokens, LPAREN);
        Token val = consume(tokens);
        node.value = val.value;
        if (val.type == IDENTIFIER)
            node.left = new Node({NODE_IDENTIFIER, val.value});
        expect(tokens, RPAREN);
        expect(tokens, SEMICOLON);
        return node;
    }
    if (peek(tokens).type == ELES) {
        Node node;
        node.type = NODE_IF;
        consume(tokens);
        expect(tokens, LPAREN);
        node.left = new Node(parseCond(tokens));
        expect(tokens, RPAREN);
        expect(tokens, LBRACE);
        node.children = parseBlock(tokens);
        expect(tokens, RBRACE);
        if (peek(tokens).type == SELE) {
            consume(tokens);
            expect(tokens, LBRACE);
            node.elseChildren = parseBlock(tokens);
            expect(tokens, RBRACE);
        }
        expect(tokens, SEMICOLON);
        return node;
    }
    std::cerr << "Неизвестная конструкция: " << peek(tokens).value << std::endl;
    exit(1);
}

std::vector<Node> parseBlock(const std::vector<Token>& tokens) {
    std::vector<Node> nodes;
    while (peek(tokens).type != RBRACE && peek(tokens).type != END) {
        nodes.push_back(parseOne(tokens));
    }
    return nodes;
}

Node parse(const std::vector<Token>& tokens) {
    pos = 0;
    Node program;
    program.type = NODE_PROGRAM;
    while (peek(tokens).type != END) {
        program.children.push_back(parseOne(tokens));
    }
    return program;
}
