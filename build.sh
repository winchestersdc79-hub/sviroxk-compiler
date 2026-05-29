#!/bin/bash
clang++ -c main.cpp -I/data/data/com.termux/files/usr/include \
  $(llvm-config --cxxflags) -fexceptions -o main.o
clang++ -c lexer.cpp -I/data/data/com.termux/files/usr/include \
  $(llvm-config --cxxflags) -fexceptions -o lexer.o
clang++ -c parser.cpp -I/data/data/com.termux/files/usr/include \
  $(llvm-config --cxxflags) -fexceptions -o parser.o
clang++ -c codegen.cpp -I/data/data/com.termux/files/usr/include \
  $(llvm-config --cxxflags) -fexceptions -o codegen.o
clang++ main.o lexer.o parser.o codegen.o \
  $(llvm-config --ldflags --libs) -o sviroxk
chmod +x run.sh build.sh test.sh 2>/dev/null || true
echo "сборка завершена"
