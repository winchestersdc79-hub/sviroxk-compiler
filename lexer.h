#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

enum TokenType {
    // Переменные
    SVI, LOR, LET, COLON,
    // Типы
    ROX, DOR, COS, BUE, VOID_T, FIXED_TYPE,
    // Условия
    ELES, SELE,
    // Циклы
    CERT, CEH, USY,
    // Функции
    FUCN, REV, COP,
    // Структуры
    TIP,
    // Вывод
    SLOV,
    // Символы
    DOT,
    EQUALS, SEMICOLON, LE, GE, AND_AND, OR_OR, BANG,
    LPAREN, RPAREN,
    LBRACE, RBRACE,
    LBRACKET, RBRACKET, COMMA,
    // Математика
    PLUS, MINUS, STAR, SLASH, PERCENT, AMP,
    PTR,
    ATK, ZAP, PRO, ZAK, FILE_T,
    SQRT, ABS, POW, MAX, MIN, FLO, CEL, RON, RAN,
    CHR, ROX64, TRUE_T, FALSE_T, NW, DEL, BRK, CONT, IMP,
    AR, SER, REP, VV_S, STR,
    VD,
    // Сравнения
    GT, LT, EQ, NEQ,
    // Значения
    NUMBER, STRING, CHAR_LITERAL, IDENTIFIER,
    END
};

struct Token {
    TokenType type;
    std::string value;
    int line = 1;
    int column = 1;
};

std::vector<Token> tokenize(const std::string& code);

#endif
