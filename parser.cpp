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
    if (peek().type == MINUS) {
        consume();
        Token num = consume();
        left.type = NODE_NUMBER;
        left.value = "-" + num.value;
        return left;
    }
    // математика
    if (peek().type == SQRT || peek().type == ABS ||
        peek().type == POW  || peek().type == MAX ||
        peek().type == MIN  || peek().type == AR) {
        Node left;
        left.type = NODE_FUNC_CALL;
        left.varName = consume().value;
        expect(LPAREN);
        while (peek().type != RPAREN && peek().type != END) {
            left.args.push_back(parseExpr());
            if (peek().type == COMMA) consume();
        }
        expect(RPAREN);
        return left;
    }
    // vd — ввод с клавиатуры
    if (peek().type == VD) {
        consume();
        Node left; left.type = NODE_INPUT;
        expect(LPAREN);
        left.value = consume().value;
        expect(RPAREN);
        return left;
    }
    // atk — открыть файл
    if (peek().type == ATK) {
        consume();
        Node left; left.type = NODE_FILE_OPEN;
        expect(LPAREN);
        left.value = consume().value; // имя файла
        expect(RPAREN);
        return left;
    }
    // pro — прочитать файл
    if (peek().type == PRO) {
        consume();
        Node left; left.type = NODE_FILE_READ;
        expect(LPAREN);
        left.varName = consume().value;
        expect(RPAREN);
        return left;
    }
    // &x — адрес
    if (peek().type == AMP) {
        consume();
        Token name = consume();
        left.type = NODE_ADDR;
        left.varName = name.value;
        return left;
    }
    // *p — разыменование
    if (peek().type == STAR) {
        consume();
        Token name = consume();
        left.type = NODE_DEREF;
        left.varName = name.value;
        return left;
    }
    Token t = consume();
    if (t.type == NUMBER)     { left.type = NODE_NUMBER;     left.value = t.value; }
    else if (t.type == STRING)     { left.type = NODE_STRING;     left.value = t.value; }
    else if (t.type == COP) {
        left.type = NODE_FUNC_CALL;
        left.varName = consume().value;
        expect(LPAREN);
        while (peek().type != RPAREN && peek().type != END) {
            left.args.push_back(parseExpr());
            if (peek().type == COMMA) consume();
        }
        expect(RPAREN);
    }
    else if (t.type == IDENTIFIER) {
        left.type = NODE_IDENTIFIER; left.value = t.value;
        if (peek().type == LBRACKET) {
            consume();
            Node* access = new Node();
            access->type = NODE_ARRAY_ACCESS;
            access->varName = t.value;
            access->left = new Node(parseExpr());
            expect(RBRACKET);
            left = *access;
        }
    }

    Token next = peek();
    if (next.value == "+" || next.value == "-" ||
        next.value == "*" || next.value == "/" ||
        next.value == "+") {
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
        consume();
        std::string varType = consume().value;
        // проверяем массив svi rox[] lor
        bool isArray = false;
        if (peek().type == LBRACKET) {
            consume(); expect(RBRACKET);
            isArray = true;
        }
        consume(); // lor
        std::string varName = consume().value;
        expect(EQUALS);
        if (isArray) {
            Node node; node.type = NODE_ARRAY_DECL;
            node.varType = varType;
            node.varName = varName;
            expect(LBRACKET);
            while (peek().type != RBRACKET && peek().type != END) {
                node.args.push_back(parseExpr());
                if (peek().type == COMMA) consume();
            }
            expect(RBRACKET);
            expect(SEMICOLON);
            return node;
        } else {
            Node node; node.type = NODE_VAR_DECL;
            node.varType = varType;
            node.varName = varName;
            node.left = new Node(parseExpr());
            expect(SEMICOLON);
            return node;
        }
    }
    // slov
    if (peek().type == SLOV) {
        Node node; node.type = NODE_SLOV;
        consume();
        expect(LPAREN);
        Node expr = parseExpr();
        node.value = expr.value;
        node.left = new Node(expr);
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
    // математика
    if (peek().type == SQRT || peek().type == ABS ||
        peek().type == POW  || peek().type == MAX ||
        peek().type == MIN  || peek().type == AR) {
        Node left;
    if (peek().type == MINUS) {
        consume();
        Token num = consume();
        left.type = NODE_NUMBER;
        left.value = "-" + num.value;
        return left;
    }
        left.type = NODE_FUNC_CALL;
        left.varName = consume().value;
        expect(LPAREN);
        while (peek().type != RPAREN && peek().type != END) {
            left.args.push_back(parseExpr());
            if (peek().type == COMMA) consume();
        }
        expect(RPAREN);
        return left;
    }
    // vd — ввод с клавиатуры
    if (peek().type == VD) {
        consume();
        Node left; left.type = NODE_INPUT;
        expect(LPAREN);
        left.value = consume().value;
        expect(RPAREN);
        return left;
    }
    // atk — открыть файл (используется в parseExpr через svi)
    // zap — записать в файл
    if (peek().type == ZAP) {
        Node node; node.type = NODE_FILE_WRITE;
        consume();
        expect(LPAREN);
        node.varName = consume().value; // имя файла
        expect(COMMA);
        node.left = new Node(parseExpr());
        expect(RPAREN);
        expect(SEMICOLON);
        return node;
    }
    // zak — закрыть файл
    if (peek().type == ZAK) {
        Node node; node.type = NODE_FILE_CLOSE;
        consume();
        expect(LPAREN);
        node.varName = consume().value;
        expect(RPAREN);
        expect(SEMICOLON);
        return node;
    }
    // *p = 10 — присваивание через указатель
    if (peek().type == STAR) {
        consume(); // *
        Node node; node.type = NODE_DEREF_ASSIGN;
        node.varName = consume().value; // имя указателя
        expect(EQUALS);
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
