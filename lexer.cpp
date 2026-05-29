#include "lexer.h"
#include <iostream>
#include <cctype>

std::vector<Token> tokenize(const std::string& code) {
    std::vector<Token> tokens;
    int i = 0;
    int line = 1;
    int column = 1;

    auto add = [&](TokenType type, const std::string& value, int tokLine, int tokCol) {
        tokens.push_back({type, value, tokLine, tokCol});
    };

    while (i < (int)code.size()) {
        if (code[i] == '\n') {
            line++;
            column = 1;
            i++;
            continue;
        }
        if (isspace(code[i])) {
            column++;
            i++;
            continue;
        }

        int tokLine = line;
        int tokCol = column;

        if (isalpha(code[i]) || code[i] == '_') {
            std::string word;
            while (i < (int)code.size() &&
                   (isalnum(code[i]) || code[i] == '_' || isdigit(code[i]))) {
                word += code[i++];
                column++;
            }
            if      (word == "svi")  add(SVI,  word, tokLine, tokCol);
            else if (word == "lor")  add(LOR,  word, tokLine, tokCol);
            else if (word == "rox")  add(ROX,  word, tokLine, tokCol);
            else if (word == "dor")  add(DOR,  word, tokLine, tokCol);
            else if (word == "cos")  add(COS,  word, tokLine, tokCol);
            else if (word == "bue")  add(BUE,  word, tokLine, tokCol);
            else if (word == "eles") add(ELES, word, tokLine, tokCol);
            else if (word == "sele") add(SELE, word, tokLine, tokCol);
            else if (word == "cert") add(CERT, word, tokLine, tokCol);
            else if (word == "ceh")  add(CEH,  word, tokLine, tokCol);
            else if (word == "usy")  add(USY,  word, tokLine, tokCol);
            else if (word == "fucn") add(FUCN, word, tokLine, tokCol);
            else if (word == "rev")  add(REV,  word, tokLine, tokCol);
            else if (word == "cop")  add(COP,  word, tokLine, tokCol);
            else if (word == "tip")  add(TIP,  word, tokLine, tokCol);
            else if (word == "slov") add(SLOV, word, tokLine, tokCol);
            else if (word == "ptr")  add(PTR,  word, tokLine, tokCol);
            else if (word == "atk")  add(ATK,  word, tokLine, tokCol);
            else if (word == "zap")  add(ZAP,  word, tokLine, tokCol);
            else if (word == "pro")  add(PRO,  word, tokLine, tokCol);
            else if (word == "zak")  add(ZAK,  word, tokLine, tokCol);
            else if (word == "file") add(FILE_T, word, tokLine, tokCol);
            else if (word == "sqrt") add(SQRT, word, tokLine, tokCol);
            else if (word == "abs")  add(ABS,  word, tokLine, tokCol);
            else if (word == "pow")  add(POW,  word, tokLine, tokCol);
            else if (word == "max")  add(MAX,  word, tokLine, tokCol);
            else if (word == "min")  add(MIN,  word, tokLine, tokCol);
            else if (word == "flo")  add(FLO,  word, tokLine, tokCol);
            else if (word == "cel")  add(CEL,  word, tokLine, tokCol);
            else if (word == "ron")  add(RON,  word, tokLine, tokCol);
            else if (word == "ran")  add(RAN,  word, tokLine, tokCol);
            else if (word == "vd")   add(VD,   word, tokLine, tokCol);
            else if (word == "nw")   add(NW,   word, tokLine, tokCol);
            else if (word == "del")  add(DEL,  word, tokLine, tokCol);
            else if (word == "brk")  add(BRK,  word, tokLine, tokCol);
            else if (word == "imp")  add(IMP,  word, tokLine, tokCol);
            else if (word == "cont") add(CONT, word, tokLine, tokCol);
            else if (word == "ar")   add(AR,   word, tokLine, tokCol);
            else if (word == "ser")  add(SER,  word, tokLine, tokCol);
            else if (word == "rep")  add(REP,  word, tokLine, tokCol);
            else if (word == "vv_s") add(VV_S, word, tokLine, tokCol);
            else if (word == "str")  add(STR,  word, tokLine, tokCol);
            else if (word == "chr")  add(CHR,  word, tokLine, tokCol);
            else if (word == "rox64") add(ROX64, word, tokLine, tokCol);
            else if (word == "true")  add(TRUE_T, word, tokLine, tokCol);
            else if (word == "false") add(FALSE_T, word, tokLine, tokCol);
            else add(IDENTIFIER, word, tokLine, tokCol);
            continue;
        }

        if (isdigit(code[i])) {
            std::string num;
            while (i < (int)code.size() && (isdigit(code[i]) || code[i] == '.')) {
                num += code[i++];
                column++;
            }
            add(NUMBER, num, tokLine, tokCol);
            continue;
        }

        if (code[i] == '\'') {
            i++;
            column++;
            int charVal = (unsigned char)code[i++];
            column++;
            i++;
            column++;
            add(NUMBER, std::to_string(charVal), tokLine, tokCol);
            continue;
        }
        if (code[i] == '"') {
            std::string str;
            i++;
            column++;
            while (i < (int)code.size() && code[i] != '"') {
                str += code[i++];
                column++;
            }
            i++;
            column++;
            add(STRING, str, tokLine, tokCol);
            continue;
        }

        char c = code[i];
        switch (c) {
            case '=':
                if (i + 1 < (int)code.size() && code[i + 1] == '=') {
                    add(EQ, "==", tokLine, tokCol);
                    i += 2;
                    column += 2;
                } else {
                    add(EQUALS, "=", tokLine, tokCol);
                    i++;
                    column++;
                }
                continue;
            case '!':
                if (i + 1 < (int)code.size() && code[i + 1] == '=') {
                    add(NEQ, "!=", tokLine, tokCol);
                    i += 2;
                    column += 2;
                }
                continue;
            case '>': add(GT, ">", tokLine, tokCol); break;
            case '<': add(LT, "<", tokLine, tokCol); break;
            case '+': add(PLUS, "+", tokLine, tokCol); break;
            case '-': add(MINUS, "-", tokLine, tokCol); break;
            case '*': add(STAR, "*", tokLine, tokCol); break;
            case '&': add(AMP, "&", tokLine, tokCol); break;
            case '/': add(SLASH, "/", tokLine, tokCol); break;
            case ';': add(SEMICOLON, ";", tokLine, tokCol); break;
            case '(': add(LPAREN, "(", tokLine, tokCol); break;
            case ')': add(RPAREN, ")", tokLine, tokCol); break;
            case '{': add(LBRACE, "{", tokLine, tokCol); break;
            case '}': add(RBRACE, "}", tokLine, tokCol); break;
            case ',': add(COMMA, ",", tokLine, tokCol); break;
            case '[': add(LBRACKET, "[", tokLine, tokCol); break;
            case ']': add(RBRACKET, "]", tokLine, tokCol); break;
            case '.': add(DOT, ".", tokLine, tokCol); break;
            default:
                std::cerr << "Ошибка " << tokLine << ":" << tokCol
                          << " неизвестный символ: " << c << std::endl;
                exit(1);
        }
        i++;
        column++;
    }

    add(END, "", line, column);
    return tokens;
}
