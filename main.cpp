#include <iostream>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

int main() {
    std::string code = "svi rox lor x = 5;";

    std::cout << "Код: " << code << std::endl;

    // Лексер
    std::vector<Token> tokens = tokenize(code);

    // Парсер
    Node node = parse(tokens);

    // Генератор
    CodeGen codegen;
    codegen.generate(node);

    std::cout << "\nLLVM IR код:" << std::endl;
    codegen.print();

    return 0;
}
