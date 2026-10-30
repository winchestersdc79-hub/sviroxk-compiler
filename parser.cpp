#include "parser.h"
#include <iostream>
#include <sstream>
#include <cstdlib>

static int pos = 0;
static std::vector<Token> gtokens;

static Token peek(size_t offset = 0) {
    if (gtokens.empty()) return {END, "", 1, 1};
    size_t index = static_cast<size_t>(pos) + offset;
    if (index >= gtokens.size()) return gtokens.back();
    return gtokens[index];
}

static Token consume() {
    Token token = peek();
    if (token.type != END && pos < static_cast<int>(gtokens.size())) ++pos;
    return token;
}

static std::string tokenDescription(const Token& token) {
    if (token.type == END) return "конец файла";
    return "'" + token.value + "'";
}

[[noreturn]] static void failAt(const Token& token, const std::string& message) {
    std::cerr << "Ошибка " << token.line << ':' << token.column << ": "
              << message << std::endl;
    std::exit(1);
}

static Token expect(TokenType type, const std::string& expected) {
    Token token = peek();
    if (token.type != type) {
        failAt(token, "ожидалось " + expected + ", получено " + tokenDescription(token));
    }
    return consume();
}

static Token expect(TokenType type) {
    return expect(type, "другой токен");
}

static bool isTypeName(const std::string& type) {
    return type == "rox" || type == "dor" || type == "cos" || type == "bue" ||
           type == "int" || type == "float" || type == "string" || type == "bool" ||
           type == "uint" || type == "short" || type == "ushort" || type == "long" ||
           type == "ulong" || type == "byte" || type == "double" || type == "char" ||
           type == "i8" || type == "i16" || type == "i32" || type == "i64" ||
           type == "u8" || type == "u16" || type == "u32" || type == "u64" ||
           type == "f32" || type == "f64" || type == "ptr" || type == "file" ||
           type == "chr" || type == "rox64";
}

static bool isTypeToken(TokenType t) {
    return t == ROX || t == DOR || t == COS || t == BUE || t == PTR ||
           t == FILE_T || t == CHR || t == ROX64 || t == FIXED_TYPE || t == IDENTIFIER;
}

static std::string canonicalType(const std::string& type) {
    if (type == "int" || type == "i32" || type == "rox") return "i32";
    if (type == "uint" || type == "u32") return "u32";
    if (type == "short" || type == "i16") return "i16";
    if (type == "ushort" || type == "u16") return "u16";
    if (type == "long" || type == "i64" || type == "rox64") return "i64";
    if (type == "ulong" || type == "u64") return "u64";
    if (type == "byte" || type == "u8") return "u8";
    if (type == "char" || type == "chr") return "char";
    if (type == "float" || type == "f32" || type == "dor") return "f32";
    if (type == "double" || type == "f64") return "f64";
    if (type == "string" || type == "str" || type == "cos") return "str";
    if (type == "bool" || type == "bue") return "bool";
    return type;
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
    if (peek().type == LPAREN &&
        (peek(1).type == ROX || peek(1).type == DOR ||
         peek(1).type == CHR || peek(1).type == ROX64)) {
        consume(); // (
        std::string castType = consume().value;
        consume(); // )
        left.type = NODE_CAST;
        left.varType = castType;
        left.left = new Node(parseExpr());
        return left;
    }
    if (peek().type == TRUE_T) {
        Token token = consume();
        left.type = NODE_NUMBER; left.value = "1";
        left.varType = "bue";
        left.line = token.line; left.column = token.column;
        return left;
    }
    if (peek().type == FALSE_T) {
        Token token = consume();
        left.type = NODE_NUMBER; left.value = "0";
        left.varType = "bue";
        left.line = token.line; left.column = token.column;
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
    left.line = t.line;
    left.column = t.column;
    if (t.type == NUMBER) {
        left.type = NODE_NUMBER;
        left.value = t.value;
        left.varType = t.value.find_first_of(".eE") == std::string::npos ? "i32" : "f64";
    }
    else if (t.type == STRING) { left.type = NODE_STRING; left.value = t.value; left.varType = "cos"; }
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
        if (peek().type == LPAREN) {
            left.type = NODE_FUNC_CALL;
            left.varName = t.value;
            left.line = t.line; left.column = t.column;
            consume();
            while (peek().type != RPAREN && peek().type != END) {
                left.args.push_back(parseExpr());
                if (peek().type == COMMA) consume();
                else if (peek().type != RPAREN)
                    failAt(peek(), "ожидалась ',' или ')' в списке аргументов");
            }
            expect(RPAREN);
        } else if (peek().type == DOT) {
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
    } else {
        failAt(t, "ожидалось выражение, получено " + tokenDescription(t));
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
    while (peek().type == STAR || peek().type == SLASH || peek().type == PERCENT) {
        std::string op = consume().value;
        Node* b = new Node();
        b->type = NODE_BINOP;
        b->op = op;
        b->line = gtokens[static_cast<size_t>(pos - 1)].line;
        b->column = gtokens[static_cast<size_t>(pos - 1)].column;
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
        c->line = op.line; c->column = op.column;
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
    // Short variable declaration: let count: int = 0;
    if (peek().type == LET) {
        Token declarationToken = consume();
        Node node; node.type = NODE_VAR_DECL;
        node.line = declarationToken.line;
        node.column = declarationToken.column;
        node.varName = expect(IDENTIFIER).value;
        expect(COLON);
        Token typeToken = consume();
        if (!isTypeName(typeToken.value))
            failAt(typeToken, "неизвестный тип '" + typeToken.value + "'");
        node.varType = canonicalType(typeToken.value);
        expect(EQUALS);
        node.left = new Node(parseExpr());
        expect(SEMICOLON);
        return node;
    }
    // svi
    if (peek().type == SVI) {
        Token declarationToken = consume();
        if (peek().type == TIP) {
            consume();
            std::string structName = consume().value;
            expect(LOR);
            std::string varName = consume().value;
            Node node;
            node.line = declarationToken.line;
            node.column = declarationToken.column;
            node.type = NODE_STRUCT_INSTANCE;
            node.varType = structName;
            node.varName = varName;
            if (peek().type == EQUALS && peek(1).type == LBRACE) {
                consume();
                expect(LBRACE);
                while (peek().type != RBRACE && peek().type != END) {
                    Node init;
                    if (peek().type == IDENTIFIER && peek(1).type == EQUALS) {
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
        Token declaredType = consume();
        if (!isTypeName(declaredType.value))
            failAt(declaredType, "неизвестный тип '" + declaredType.value + "'");
        std::string varType = canonicalType(declaredType.value);
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
            node.line = declarationToken.line;
            node.column = declarationToken.column;
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
            node.line = declarationToken.line;
            node.column = declarationToken.column;
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
        node.line = peek().line; node.column = peek().column;
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
        node.line = peek().line;
        node.column = peek().column;
        consume();
        node.varType = "rox";
        if (peek().type == VOID_T) {
            consume();
            node.varType = "void";
        } else if (isTypeToken(peek().type) && peek(1).type == IDENTIFIER &&
                   peek(2).type == LPAREN) {
            Token returnType = consume();
            if (!isTypeName(returnType.value))
                failAt(returnType, "неизвестный тип результата '" + returnType.value + "'");
            node.varType = canonicalType(returnType.value);
        }
        node.varName = consume().value;
        expect(LPAREN);
        while (peek().type != RPAREN && peek().type != END) {
            Token typeToken = consume();
            if (!isTypeToken(typeToken.type))
                failAt(typeToken, "ожидался тип параметра функции");
            if (!isTypeName(typeToken.value))
                failAt(typeToken, "неизвестный тип параметра '" + typeToken.value + "'");
            std::string ptype = canonicalType(typeToken.value);
            std::string pname = expect(IDENTIFIER, "имя параметра").value;
            node.params.push_back(ptype + ":" + pname);
            if (peek().type == COMMA) {
                consume();
                if (peek().type == RPAREN)
                    failAt(peek(), "после запятой ожидался параметр");
            } else if (peek().type != RPAREN) {
                failAt(peek(), "ожидалась ',' или ')' после параметра");
            }
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
        node.line = peek().line; node.column = peek().column;
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
        node.line = peek().line; node.column = peek().column;
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
    if (peek().type == IDENTIFIER && peek(1).type == DOT &&
        peek(3).type == EQUALS) {
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
    if (peek().type == IDENTIFIER && peek(1).type == LBRACKET) {
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
    // вызов функции как отдельный оператор
    if (peek().type == IDENTIFIER && peek(1).type == LPAREN) {
        Node node; node.type = NODE_FUNC_CALL;
        Token name = consume();
        node.varName = name.value;
        node.line = name.line; node.column = name.column;
        expect(LPAREN);
        while (peek().type != RPAREN && peek().type != END) {
            node.args.push_back(parseExpr());
            if (peek().type == COMMA) consume();
            else if (peek().type != RPAREN)
                failAt(peek(), "ожидалась ',' или ')' в списке аргументов");
        }
        expect(RPAREN);
        expect(SEMICOLON);
        return node;
    }
    // Удобное объявление с явным типом: int count = 1;
    if (isTypeName(peek().value) &&
        (peek().type != IDENTIFIER || peek(1).type == IDENTIFIER)) {
        Token typeToken = consume();
        if (!isTypeName(typeToken.value))
            failAt(typeToken, "неизвестный тип '" + typeToken.value + "'");
        Node node; node.type = NODE_VAR_DECL;
        node.line = typeToken.line; node.column = typeToken.column;
        node.varType = canonicalType(typeToken.value);
        node.varName = expect(IDENTIFIER, "имя переменной").value;
        expect(EQUALS, "'='");
        node.left = new Node(parseExpr());
        expect(SEMICOLON, "';'");
        return node;
    }
    // присваивание
    if (peek().type == IDENTIFIER && peek(1).type == EQUALS) {
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
        node.line = peek().line;
        node.column = peek().column;
        consume();
        node.varName = consume().value;
        expect(LBRACE);
        while (peek().type != RBRACE && peek().type != END) {
            Node field; field.type = NODE_VAR_DECL;
            Token fieldType = consume();
            if (!isTypeName(fieldType.value))
                failAt(fieldType, "неизвестный тип поля '" + fieldType.value + "'");
            field.varType = canonicalType(fieldType.value);
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

    failAt(peek(), "неизвестная конструкция, начинается с " + tokenDescription(peek()));
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
