#include "parser.h"
#include <iostream>

static int pos = 0;
static std::vector<Token> gtokens;

Token peek() { return gtokens[pos]; }
Token consume() { return gtokens[pos++]; }

Token expect(TokenType type) {
    Token t = consume();
    if (t.type != type) {
        std::cerr << "Ошибка pos=" << pos << " ожидал=" << type << " prev=[" << gtokens[pos-2].value << "] got=[" << t.value << "]" << std::endl;
        exit(1);
    }
    return t;
}

Node parseExpr();
std::vector<Node> parseBlock();

Node parseExpr() {
    Node left;
    Token t = consume();
    if (t.type == NUMBER)     { left.type = NODE_NUMBER;     left.value = t.value; }
    else if (t.type == STRING)     { left.type = NODE_STRING;     left.value = t.value; }
    else if (t.type == IDENTIFIER) { left.type = NODE_IDENTIFIER; left.value = t.value; }

    Token next = peek();
    if (next.value == "+" || next.value == "-" ||
        next.value == "*" || next.value == "/") {
        consume();
        Node* b = new Node();
        b->type = NODE_BINOP;
        b->op = next.value;
        b->left = new Node(left);
        b->right = new Node(parseExpr());
        return *b;
    }
    return left;
}

Node parseCond() {
    Node left = parseExpr();
    Token op = consume();
    Node right = parseExpr();
    Node* c = new Node();
    c->type = NODE_BINOP;
    c->op = op.value;
    c->left = new Node(left);
    c->right = new Node(right);
    return *c;
}

Node parseOne();

std::vector<Node> parseBlock() {
    std::vector<Node> nodes;
    while (peek().type != RBRACE && peek().type != END) {
        if (peek().type == SEMICOLON) { consume(); continue; }
        nodes.push_back(parseOne());
    }
    return nodes;
}

Node parseOne() {
    // svi
    if (peek().type == SVI) {
        Node node; node.type = NODE_VAR_DECL;
        consume();
        node.varType = consume().value;
        consume(); // lor
        node.varName = consume().value;
        expect(EQUALS);
        node.left = new Node(parseExpr());
        expect(SEMICOLON);
        return node;
    }
    // slov
    if (peek().type == SLOV) {
        Node node; node.type = NODE_SLOV;
        consume();
        expect(LPAREN);
        Token val = consume();
        node.value = val.value;
        if (val.type == IDENTIFIER)
            node.left = new Node({NODE_IDENTIFIER, val.value});
        expect(RPAREN);
        expect(SEMICOLON);
        return node;
    }
    // fucn
    if (peek().type == FUCN) {
        Node node; node.type = NODE_FUNC_DEF;
        consume();
        node.varName = consume().value;
        expect(LPAREN);
        while (peek().type != RPAREN && peek().type != END) {
            std::string ptype = consume().value;
            std::string pname = consume().value;
            node.params.push_back(ptype + ":" + pname);
            if (peek().type == COMMA) consume();
        }
        expect(RPAREN);
        expect(LBRACE);
        node.children = parseBlock();
        expect(RBRACE);
        if (peek().type == SEMICOLON) consume();
        return node;
    }
    // cop
    if (peek().type == COP) {
        Node node; node.type = NODE_FUNC_CALL;
        consume();
        node.varName = consume().value;
        expect(LPAREN);
        while (peek().type != RPAREN && peek().type != END) {
            node.args.push_back(parseExpr());
            if (peek().type == COMMA) consume();
        }
        expect(RPAREN);
        expect(SEMICOLON);
        return node;
    }
    // rev
    if (peek().type == REV) {
        Node node; node.type = NODE_RETURN;
        consume();
        node.left = new Node(parseExpr());
        expect(SEMICOLON);
        return node;
    }
    // присваивание
    if (peek().type == IDENTIFIER && gtokens[pos+1].type == EQUALS) {
        Node node; node.type = NODE_ASSIGN;
        node.varName = consume().value;
        consume(); // =
        node.left = new Node(parseExpr());
        expect(SEMICOLON);
        return node;
    }
    // cert
    if (peek().type == CERT) {
        Node node;
        consume();
        Token kind = consume();
        node.type = (kind.type == CEH) ? NODE_LOOP_N : NODE_LOOP_W;
        expect(LPAREN);
        node.left = new Node(parseExpr());
        if (node.type == NODE_LOOP_W) {
            Token op = consume();
            Node right = parseExpr();
            Node* cond = new Node();
            cond->type = NODE_BINOP;
            cond->op = op.value;
            cond->left = node.left;
            cond->right = new Node(right);
            node.left = cond;
        }
        expect(RPAREN);
        expect(LBRACE);
        node.children = parseBlock();
        expect(RBRACE);
        if (peek().type == SEMICOLON) consume();
        return node;
    }
    // eles
    if (peek().type == ELES) {
        Node node; node.type = NODE_IF;
        consume();
        expect(LPAREN);
        node.left = new Node(parseCond());
        expect(RPAREN);
        expect(LBRACE);
        node.children = parseBlock();
        expect(RBRACE);
        if (peek().type == SELE) {
            consume();
            expect(LBRACE);
            node.elseChildren = parseBlock();
            expect(RBRACE);
        }
        if (peek().type == SEMICOLON) consume();
        return node;
    }
    // tip
    if (peek().type == TIP) {
        Node node; node.type = NODE_STRUCT_DEF;
        consume();
        node.varName = consume().value;
        expect(LBRACE);
        while (peek().type != RBRACE && peek().type != END) {
            Node field; field.type = NODE_VAR_DECL;
            field.varType = consume().value;
            field.varName = consume().value;
            expect(EQUALS);
            field.left = new Node(parseExpr());
            expect(SEMICOLON);
            node.children.push_back(field);
        }
        expect(RBRACE);
        if (peek().type == SEMICOLON) consume();
        return node;
    }

    std::cerr << "Неизвестная конструкция: [" << peek().value << "] pos=" << pos << std::endl;
    exit(1);
}

Node parse(const std::vector<Token>& tokens) {
    gtokens = tokens;
    pos = 0;
    Node program;
    program.type = NODE_PROGRAM;
    while (peek().type != END) {
        program.children.push_back(parseOne());
    }
    return program;
}
