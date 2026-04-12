#include "lexer.h"
#include <iostream>
#include <cctype>

std::vector<Token> tokenize(const std::string& code) {
    std::vector<Token> tokens;
    int i = 0;

    while (i < code.size()) {
        if (isspace(code[i])) { i++; continue; }

        if (isalpha(code[i]) || code[i] == '_') {
            std::string word;
            while (i < code.size() && (isalnum(code[i]) || code[i] == '_'))
                word += code[i++];

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
            else if (word == "slov") tokens.push_back({SLOV, word});
            else if (word == "ptr")  tokens.push_back({PTR,  word});
            else if (word == "atk")  tokens.push_back({ATK,  word});
            else if (word == "zap")  tokens.push_back({ZAP,  word});
            else if (word == "pro")  tokens.push_back({PRO,  word});
            else if (word == "zak")  tokens.push_back({ZAK,  word});
            else if (word == "file") tokens.push_back({FILE_T, word});
            else if (word == "sqrt") tokens.push_back({SQRT, word});
            else if (word == "abs")  tokens.push_back({ABS,  word});
            else if (word == "pow")  tokens.push_back({POW,  word});
            else if (word == "max")  tokens.push_back({MAX,  word});
            else if (word == "min")  tokens.push_back({MIN,  word});
            else if (word == "vd")  tokens.push_back({VD,  word});
            else if (word == "ar")  tokens.push_back({AR,  word});
            else tokens.push_back({IDENTIFIER, word});
            continue;
        }

        if (isdigit(code[i])) {
            std::string num;
            while (i < code.size() && (isdigit(code[i]) || code[i] == '.'))
                num += code[i++];
            tokens.push_back({NUMBER, num});
            continue;
        }

        if (code[i] == '"') {
            std::string str;
            i++;
            while (i < code.size() && code[i] != '"')
                str += code[i++];
            i++;
            tokens.push_back({STRING, str});
            continue;
        }

        switch (code[i]) {
            case '=':
                if (code[i+1] == '=') {
                    tokens.push_back({EQ, "=="});
                    i += 2;
                } else {
                    tokens.push_back({EQUALS, "="});
                    i++;
                }
                continue;
            case '!':
                if (code[i+1] == '=') {
                    tokens.push_back({NEQ, "!="});
                    i += 2;
                }
                continue;
            case '>': tokens.push_back({GT,       ">"}); break;
            case '<': tokens.push_back({LT,       "<"}); break;
            case '+': tokens.push_back({PLUS,     "+"}); break;
            case '-': tokens.push_back({MINUS,    "-"}); break;
            case '*': tokens.push_back({STAR,     "*"}); break;
            case '&': tokens.push_back({AMP,      "&"}); break;
            case '/': tokens.push_back({SLASH,    "/"}); break;
            case ';': tokens.push_back({SEMICOLON,";"}); break;
            case '(': tokens.push_back({LPAREN,   "("}); break;
            case ')': tokens.push_back({RPAREN,   ")"}); break;
            case '{': tokens.push_back({LBRACE,   "{"}); break;
            case '}': tokens.push_back({RBRACE,   "}"}); break;
            case ',': tokens.push_back({COMMA,    ","}); break;
            case '[': tokens.push_back({LBRACKET, "["}); break;
            case ']': tokens.push_back({RBRACKET, "]"}); break;
            default:
                std::cerr << "Неизвестный символ: " << code[i] << std::endl;
                exit(1);
        }
        i++;
    }

    tokens.push_back({END, ""});
    return tokens;
}
