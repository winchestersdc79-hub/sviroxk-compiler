#include <iostream>
#include <fstream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Использование: sviroxk <файл.svx>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Файл не найден: " << argv[1] << std::endl;
        return 1;
    }
    std::string code((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
    file.close();

    std::vector<Token> tokens = tokenize(code);
    Node node = parse(tokens);

    CodeGen codegen;
    codegen.generate(node);
    codegen.print();

    return 0;
}
