#!/data/data/com.termux/files/usr/bin/bash
set -e

echo -e "\033[1;32m=== Сборка SVIROXK compiler ===\033[0m"

clang++ -std=c++17 -O2 -o sviroxk \
    main.cpp lexer.cpp parser.cpp codegen.cpp \
    -lLLVM \
    $(llvm-config --cxxflags --ldflags --libs core irreader support) 2>&1 | tee build.log

if [ ${PIPESTATUS[0]} -eq 0 ]; then
    echo -e "\033[1;32m✓ Компилятор собран успешно!\033[0m"
    if [ -f "hello.svx" ]; then
        echo -e "\033[1;34m→ Компиляция hello.svx...\033[0m"
        ./sviroxk hello.svx
        if [ -f "output" ]; then
            echo -e "\033[1;32m✓ Запуск программы:\033[0m"
            ./output
        fi
    fi
else
    echo -e "\033[1;31m✗ Ошибка сборки!\033[0m"
    cat build.log
fi
