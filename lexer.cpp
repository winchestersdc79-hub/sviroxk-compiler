#include "lexer.h"
#include <cctype>
#include <stdexcept>

std::vector<Token> tokenize(const std::string& code) {
    std::vector<Token> tokens;
    int i = 0;

    while (i < code.size()) {
        
        // Пропускаем пробелы
        if (isspace(code[i])) {
            i++;
            continue;
        }

        // Ключевые слова и идентификаторы
        if (isalpha(code[i])) {
            std::string word;
            while (i < code.size() && isalnum(code[i])) {
                word += code[i++];
            }

            if      (word == "svi")  tokens.push_back({SVI,  word});
            else if (word == "lor")  tokens.push_back({LOR,  word});
            else if (word == "rox")  tokens.push_back({ROX,  word});
            else if (word == "dor")  tokens.push_back({DOR,  word});
            else if (word == "cos")  tokens.push_back({COS,  word});
            else if (word == "bue")  tokens.push_back({BUE,  word});
            else if (word == "eles") tokens.push_back({ELES, word});
            else if (word == "sele") tokens.push_back({SELE, word});
            else if (word == "cert") tokens.push_back({CERT, word});
            else if (word == "ceh")  tokens.push_back({CEH,  word});
            else if (word == "usy")  tokens.push_back({USY,  word});
            else if (word == "fucn") tokens.push_back({FUCN, word});
            else if (word == "rev")  tokens.push_back({REV,  word});
            else if (word == "cop")  tokens.push_back({COP,  word});
            else if (word == "tip")  tokens.push_back({TIP,  word});
            else if (word == "true" || word == "false")
                tokens.push_back({IDENTIFIER, word});
            else
                tokens.push_back({IDENTIFIER, word});

            continue;
        }

        // Числа
        if (isdigit(code[i])) {
            std::string num;
            while (i < code.size() && (isdigit(code[i]) || code[i] == '.')) {
                num += code[i++];
            }
            tokens.push_back({NUMBER, num});
            continue;
        }

        // Строки в кавычках
        if (code[i] == '"') {
            std::string str;
            i++; // пропускаем открывающую кавычку
            while (i < code.size() && code[i] != '"') {
                str += code[i++];
            }
            i++; // пропускаем закрывающую кавычку
            tokens.push_back({STRING, str});
            continue;
        }

        // Символы
        switch (code[i]) {
            case '=': tokens.push_back({EQUALS,    "="}); break;
            case ';': tokens.push_back({SEMICOLON, ";"}); break;
            case '(': tokens.push_back({LPAREN,    "("}); break;
            case ')': tokens.push_back({RPAREN,    ")"}); break;
            case '{': tokens.push_back({LBRACE,    "{"}); break;
            case '}': tokens.push_back({RBRACE,    "}"}); break;
            case '[': tokens.push_back({LBRACKET,  "["}); break;
            case ']': tokens.push_back({RBRACKET,  "]"}); break;
            default:
                throw std::runtime_error(
                    std::string("Неизвестный символ: ") + code[i]
                );
        }
        i++;
    }

    tokens.push_back({END, ""});
    return tokens;
}
