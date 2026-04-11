#include <iostream>
#include "lexer.h"

int main() {
    // Тестовый код на SVIROXK
    std::string code = "svi rox lor x = 5;";

    std::cout << "Код: " << code << std::endl;
    std::cout << "Токены:" << std::endl;

    // Запускаем лексер
    std::vector<Token> tokens = tokenize(code);

    // Выводим все токены
    for (Token t : tokens) {
        std::cout << "  [" << t.value << "]" << std::endl;
    }

    return 0;
}
