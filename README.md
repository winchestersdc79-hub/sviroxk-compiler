Sviroxk 
🧠 About
sviroxk — это разрабатываемый язык программирования и компилятор, создаваемый с нуля.
Проект следует классической модели разработки языков:
сначала компилятор реализуется на C++ (bootstrap), а затем язык развивается до уровня, при котором способен компилировать сам себя (self-hosting).
🎯 Goal
Главная цель проекта — создать:
независимый язык программирования
собственный компилятор
полностью самодостаточную систему
После достижения self-hosting этапа язык больше не будет зависеть от C++.
🔄 Development Stages
Stage 1 — Bootstrap (Current)
Компилятор написан на C++
Реализуется базовый синтаксис
Создаются lexer и parser
Формируется архитектура
Stage 2 — Language Growth
Добавление переменных и типов
Реализация функций
Улучшение синтаксиса
Повышение стабильности
Stage 3 — Self-hosting (Target)
Компилятор переписывается на sviroxk
sviroxk компилирует сам себя
Удаляется зависимость от C++
🏗 Compiler Architecture
Компилятор включает стандартные этапы:
Lexer — разбиение кода на токены
Parser — построение структуры программы
AST — абстрактное синтаксическое дерево
Code Generation — генерация кода
⚙️ Build
Bash
Копировать код
g++ -o sviroxk main.cpp
▶️ Usage
Bash
Копировать код
./sviroxk program.svx
📁 Project Structure (example)
Копировать код

sviroxk/
│
├── src/
├── lexer/
├── parser/
├── codegen/
├── include/
└── main.cpp
💡 Example (pseudo sviroxk)
Sviroxk
Копировать код
func main() {
    print("Hello, sviroxk")
}
🚧 Status
Проект находится на ранней стадии разработки.
Функциональность и синтаксис могут изменяться.
🔮 Roadmap
[ ] Базовый синтаксис
[ ] Переменные
[ ] Типы данных
[ ] Функции
[ ] AST
[ ] Генерация кода
[ ] Self-hosting компилятор
📄 License
This project uses a custom restrictive license.
Usage Terms
You are allowed to use this software for personal and educational purposes only.
You are NOT allowed to modify the source code.
You are NOT allowed to distribute this software.
You are NOT allowed to sell or use this software commercially.
Disclaimer
This software is provided "as is", without warranty of any kind.
📌 Notes
Проект не является open-source из-за ограничений лицензии
C++ используется только как временный этап (bootstrap)
Основная цель — полный переход на sviroxk
🧬 Summary
sviroxk — это путь от простого компилятора на C++
к полноценному языку, который способен развивать сам себя.
