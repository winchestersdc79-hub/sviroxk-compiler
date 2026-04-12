# sviroxk

## About
sviroxk — это разрабатываемый язык программирования и компилятор, создаваемый с нуля. Проект следует классической модели разработки языков: сначала компилятор реализуется на C++ (bootstrap), а затем язык развивается до уровня self-hosting, где он сможет компилировать сам себя.

## Goal
Главная цель проекта — создать независимый язык программирования, собственный компилятор и полностью самодостаточную систему. После перехода на self-hosting C++ больше не будет использоваться.

## Development Stages

### Stage 1 — Bootstrap (Current)
Компилятор написан на C++. Реализуется базовый синтаксис, lexer, parser и основная архитектура.

### Stage 2 — Language Growth
Добавление переменных, типов, функций, улучшение синтаксиса и стабильности.

### Stage 3 — Self-hosting (Target)
Компилятор переписывается на sviroxk, язык компилирует сам себя, C++ больше не нужен.

## Compiler Architecture
- Lexer — разбиение кода на токены
- Parser — построение структуры программы
- AST — абстрактное синтаксическое дерево
- Code Generation — генерация кода

## Build
g++ -o sviroxk main.cpp

## Usage
./sviroxk program.svx

## Project Structure
sviroxk/
├── src/
├── lexer/
├── parser/
├── codegen/
├── include/
└── main.cpp

## Example
func main() {
print("Hello, sviroxk")
}

## Status
Проект на ранней стадии разработки. Синтаксис и архитектура могут изменяться.

## Roadmap
- Базовый синтаксис
- Переменные
- Типы данных
- Функции
- AST
- Генерация кода
- Self-hosting компилятор

## License
This project uses a custom restrictive license.

Usage Terms:
- Only personal and educational use allowed
- No modification allowed
- No distribution allowed
- No commercial use allowed

Disclaimer:
Provided "as is" without warranty.

## Notes
C++ используется только как временный bootstrap. Основная цель — полный переход на sviroxk и self-hosting.
