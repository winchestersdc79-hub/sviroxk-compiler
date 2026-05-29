#include "parser.h"
#include <iostream>

static int pos = 0;
static std::vector<Token> gtokens;

Token peek() { return gtokens[pos]; }
Token consume() { return gtokens[pos++]; }

Token expect(TokenType type) {
    Token at = peek();
    Token t = consume();
    if (t.type != type) {
        std::cerr << "Ошибка " << at.line << ":" << at.column
                  << " ожидался другой токен, получено [" << t.value << "]"
                  << std::endl;
        exit(1);
    }
    return t;
}

static bool isTypeToken(TokenType t) {
    return t == ROX || t == DOR || t == COS || t == BUE || t == PTR ||
           t == FILE_T || t == CHR || t == ROX64;
}

Node parseExpr();
Node parsePrimary();
Node parseUnary();
Node parseMul();
Node parseAdd();
std::vector<Node> parseBlock();

Node parsePrimary() {
    Node left;
    // (dor)x — приведение типов
    if (peek().type == LPAREN && pos+1 < (int)gtokens.size() &&
        (gtokens[pos+1].type == ROX || gtokens[pos+1].type == DOR ||
         gtokens[pos+1].type == CHR || gtokens[pos+1].type == ROX64)) {
        consume(); // (
        std::string castType = consume().value;
        consume(); // )
        left.type = NODE_CAST;
        left.varType = castType;
        left.left = new Node(parseExpr());
        return left;
    }
    if (peek().type == TRUE_T) {
        consume();
        left.type = NODE_NUMBER; left.value = "1";
        return left;
    }
    if (peek().type == FALSE_T) {
        consume();
        left.type = NODE_NUMBER; left.value = "0";
        return left;
    }
    // математика
    if (peek().type == SQRT || peek().type == ABS ||
        peek().type == POW  || peek().type == MAX ||
        peek().type == MIN  || peek().type == AR  ||
        peek().type == FLO  || peek().type == CEL ||
        peek().type == RON  || peek().type == RAN ||
        peek().type == SER  || peek().type == REP ||
        peek().type == STR) {
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
    // vv_s — ввод строки (в выражении)
    if (peek().type == VV_S) {
        consume();
        Node left; left.type = NODE_INPUT;
        left.varType = "cos";
        expect(LPAREN);
        left.value = consume().value;
        expect(RPAREN);
        return left;
    }
    // nw — создать динамический массив
    if (peek().type == NW) {
        consume();
        Node left; left.type = NODE_DYN_ARRAY;
        left.varType = consume().value; // тип (rox etc)
        expect(LBRACKET);
        left.left = new Node(parseExpr()); // размер
        expect(RBRACKET);
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
        if (peek().type == DOT) {
            consume();
            Token field = consume();
            left.type = NODE_STRUCT_FIELD;
            left.varName = t.value;
            left.op = field.value;
        } else if (peek().type == LBRACKET) {
            consume();
            Node* access = new Node();
            access->type = NODE_ARRAY_ACCESS;
            access->varName = t.value;
            access->left = new Node(parseExpr());
            expect(RBRACKET);
            left = *access;
        }
    }
    return left;
}

Node parseUnary() {
    if (peek().type == MINUS) {
        consume();
        if (peek().type == NUMBER) {
            Token num = consume();
            Node n;
            n.type = NODE_NUMBER;
            n.value = "-" + num.value;
            return n;
        }
        Node n;
        n.type = NODE_BINOP;
        n.op = "-";
        n.left = new Node();
        n.left->type = NODE_NUMBER;
        n.left->value = "0";
        n.right = new Node(parseUnary());
        return n;
    }
    return parsePrimary();
}

// * и / — умножение и деление (+ и - в parseAdd). Символы те же: + - * /
Node parseMul() {
    Node left = parseUnary();
    while (peek().type == STAR || peek().type == SLASH) {
        std::string op = consume().value;
        Node* b = new Node();
        b->type = NODE_BINOP;
        b->op = op;
        b->left = new Node(left);
        b->right = new Node(parseUnary());
        left = *b;
    }
    return left;
}

Node parseAdd() {
    Node left = parseMul();
    while (peek().type == PLUS || peek().type == MINUS) {
        std::string op = consume().value;
        Node* b = new Node();
        b->type = NODE_BINOP;
        b->op = op;
        b->left = new Node(left);
        b->right = new Node(parseMul());
        left = *b;
    }
    return left;
}

Node parseExpr() {
    return parseAdd();
}

Node parseCond() {
    Node left = parseAdd();
    if (peek().type == GT || peek().type == LT ||
        peek().type == EQ || peek().type == NEQ) {
        Token op = consume();
        Node right = parseAdd();
        Node* c = new Node();
        c->type = NODE_BINOP;
        c->op = op.value;
        c->left = new Node(left);
        c->right = new Node(right);
        return *c;
    }
    return left;
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
        if (peek().type == TIP) {
            consume();
            std::string structName = consume().value;
            expect(LOR);
            std::string varName = consume().value;
            Node node;
            node.type = NODE_STRUCT_INSTANCE;
            node.varType = structName;
            node.varName = varName;
            if (peek().type == EQUALS && pos + 1 < (int)gtokens.size() &&
                gtokens[pos + 1].type == LBRACE) {
                consume();
                expect(LBRACE);
                while (peek().type != RBRACE && peek().type != END) {
                    Node init;
                    if (peek().type == IDENTIFIER &&
                        pos + 1 < (int)gtokens.size() &&
                        gtokens[pos + 1].type == EQUALS) {
                        init.varName = consume().value;
                        consume();
                        init.left = new Node(parseExpr());
                    } else {
                        init.left = new Node(parseExpr());
                    }
                    node.children.push_back(init);
                    if (peek().type == COMMA || peek().type == SEMICOLON)
                        consume();
                }
                expect(RBRACE);
            }
            expect(SEMICOLON);
            return node;
        }
        std::string varType = consume().value;
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
        node.varType = "rox";
        consume();
        if (isTypeToken(peek().type) && pos + 1 < (int)gtokens.size() &&
            gtokens[pos + 1].type == IDENTIFIER &&
            pos + 2 < (int)gtokens.size() &&
            gtokens[pos + 2].type == LPAREN) {
            node.varType = consume().value;
        }
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
    // мат. функции / str как оператор
    if (peek().type == SQRT || peek().type == ABS ||
        peek().type == POW  || peek().type == MAX ||
        peek().type == MIN  || peek().type == AR  ||
        peek().type == FLO  || peek().type == CEL ||
        peek().type == RON  || peek().type == RAN ||
        peek().type == SER  || peek().type == REP ||
        peek().type == STR) {
        Node node = parseExpr();
        expect(SEMICOLON);
        return node;
    }
    // vd — ввод числа (оператор)
    if (peek().type == VD) {
        Node node = parseExpr();
        expect(SEMICOLON);
        return node;
    }
    // vv_s — ввод строки (оператор)
    if (peek().type == VV_S) {
        consume();
        Node node; node.type = NODE_INPUT;
        node.varType = "cos"; // строка
        expect(LPAREN);
        node.value = consume().value;
        expect(RPAREN);
        expect(SEMICOLON);
        return node;
    }
    // brk — выход из цикла
    if (peek().type == BRK) {
        consume();
        expect(SEMICOLON);
        Node node; node.type = NODE_BREAK;
        return node;
    }
    // cont — следующая итерация
    if (peek().type == CONT) {
        consume();
        expect(SEMICOLON);
        Node node; node.type = NODE_CONTINUE;
        return node;
    }
    // del — удалить массив
    if (peek().type == DEL) {
        Node node; node.type = NODE_DELETE;
        consume();
        node.varName = consume().value;
        expect(SEMICOLON);
        return node;
    }
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
    // p.health = 10 — поле структуры
    if (peek().type == IDENTIFIER && pos + 3 < (int)gtokens.size() &&
        gtokens[pos + 1].type == DOT && gtokens[pos + 3].type == EQUALS) {
        std::string inst = consume().value;
        consume();
        std::string field = consume().value;
        Node node;
        node.type = NODE_ASSIGN;
        node.varName = inst + "_" + field;
        expect(EQUALS);
        node.left = new Node(parseExpr());
        expect(SEMICOLON);
        return node;
    }
    // arr[0] = 10 — присваивание элемента массива
    if (peek().type == IDENTIFIER && pos+1 < (int)gtokens.size() && gtokens[pos+1].type == LBRACKET) {
        Node node; node.type = NODE_ARRAY_ASSIGN;
        node.varName = consume().value;
        consume(); // [
        node.left = new Node(parseExpr());
        expect(RBRACKET);
        expect(EQUALS);
        node.right = new Node(parseExpr());
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

    std::cerr << "Ошибка " << peek().line << ":" << peek().column
              << " неизвестная конструкция: [" << peek().value << "]" << std::endl;
    exit(1);
}

Node parse(const std::vector<Token>& tokens) {
    gtokens = tokens;
    pos = 0;
    Node program;
    program.type = NODE_PROGRAM;
    while (peek().type != END) {
        if (peek().type == IMP) {
            consume();
            Node node; node.type = NODE_IMPORT;
            node.value = consume().value; // имя файла
            expect(SEMICOLON);
            program.children.push_back(node);
        } else {
            program.children.push_back(parseOne());
        }
    }
    return program;
}
