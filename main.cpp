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

    // обрабатываем импорты
    std::vector<Token> allTokens;
    for (int i = 0; i < (int)tokens.size(); i++) {
        if (tokens[i].type == IMP && i+1 < (int)tokens.size()) {
            std::string importFile = tokens[i+1].value;
            std::ifstream impFile(importFile);
            if (impFile.is_open()) {
                std::string impCode((std::istreambuf_iterator<char>(impFile)),
                    std::istreambuf_iterator<char>());
                impFile.close();
                std::vector<Token> impTokens = tokenize(impCode);
                impTokens.pop_back(); // убираем END
                for (auto& t : impTokens) allTokens.push_back(t);
            }
            i += 2; // пропускаем IMP и имя файла и ;
        } else {
            allTokens.push_back(tokens[i]);
        }
    }

    Node node = parse(allTokens);

    CodeGen codegen;
    codegen.generate(node);
    codegen.print();

    return 0;
}
