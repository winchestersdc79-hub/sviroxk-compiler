#include <iostream>
#include "lexer.h"
#include "parser.h"

int main() {
    // Тестовый код на SVIROXK
    std::string code = "svi rox lor x = 5;";

    std::cout << "Код: " << code << std::endl;

    // Лексер
    std::vector<Token> tokens = tokenize(code);
    std::cout << "\nТокены:" << std::endl;
    for (Token t : tokens) {
        if (t.value != "")
            std::cout << "  [" << t.value << "]" << std::endl;
    }

    // Парсер
    Node node = parse(tokens);
    std::cout << "\nДерево:" << std::endl;
    std::cout << "  Тип узла : объявление переменной" << std::endl;
    std::cout << "  Тип данных: " << node.varType << std::endl;
    std::cout << "  Имя      : " << node.varName << std::endl;
    std::cout << "  Значение : " << node.value << std::endl;

    return 0;
}
