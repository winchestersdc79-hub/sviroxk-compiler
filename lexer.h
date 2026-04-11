#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

// Все типы токенов языка SVIROXK
enum TokenType {
    // Переменные
    SVI,        // svi
    LOR,        // lor
    
    // Типы данных
    ROX,        // int
    DOR,        // float
    COS,        // string
    BUE,        // bool
    
    // Условия
    ELES,       // if
    SELE,       // else
    
    // Циклы
    CERT,       // cert
    CEH,        // ceh (число)
    USY,        // usy (условие)
    
    // Функции
    FUCN,       // fucn
    REV,        // return
    COP,        // вызов функции
    
    // Структуры
    TIP,        // struct
    SLOV,       // вывод текста    
    // Символы
    EQUALS,     // =
    SEMICOLON,  // ;
    LPAREN,     // (
    RPAREN,     // )
    LBRACE,     // {
    RBRACE,     // }
    LBRACKET,   // [
    RBRACKET,   // ]
    
    // Значения
    NUMBER,     // 5, 3.14
    STRING,     // "текст"
    IDENTIFIER, // имена переменных
    
    END         // конец файла
};

// Структура токена
struct Token {
    TokenType type;
    std::string value;
};

// Функция лексера
std::vector<Token> tokenize(const std::string& code);

#endif
