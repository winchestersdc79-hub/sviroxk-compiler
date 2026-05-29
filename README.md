# SVIROXK

SVIROXK — язык программирования созданный с нуля. Компилятор написан на C++ с использованием LLVM для генерации машинного кода.

## Цель

Создать полностью независимый язык программирования и компилятор. Сейчас в работе **Этап 1** (рабочий язык как Python/C++). Этапы 3–4 (движок, игра) — по желанию, позже.

## Этапы разработки

- **Этап 1 — Компилятор на C++** (текущий) — лексер, парсер, LLVM, синтаксис из README
- **Этап 2 — Самохостинг** — компилятор на SVIROXK
- **Этап 3–4** — движок и игра (опционально)

Подробный чеклист: [STATUS.md](STATUS.md)

## Синтаксис

### Переменные
svi rox lor x = 5;
svi dor lor y = 3.14;
svi cos lor name = "SVIROXK";
svi bue lor flag = true;
### Массивы
svi rox[] lor arr = [1, 2, 3];
slov(arr[0]);
### Условия
eles (x > 5) {
slov("больше");
} sele {
slov("меньше");
}
### Циклы
cert ceh (10) {
slov("повтор");
}
cert usy (x < 10) {
x = x + 1;
}
### Функции
fucn add(rox a, rox b) {
rev a + b;
}
svi rox lor result = cop add(5, 3);
### Операторы

Символы как везде: `+` плюс, `-` минус, `*` умножение, `/` деление.  
Приоритет: сначала `*` и `/`, потом `+` и `-` (как в C и Python).

### Структуры
tip Player {
    rox health = 100;
    rox speed = 5;
}
svi tip Player lor hero;
hero.health = 80;
slov(hero.health);
### Указатели
svi rox lor x = 5;
svi ptr lor p = &x;
*p = 10;
slov(*p);
### Файлы
svi file lor f = atk("file.txt");
zap(f, "текст");
zak(f);
## Типы данных

| SVIROXK | C++ | Описание |
|---------|-----|----------|
| rox | int | целое число |
| dor | float | дробное число |
| cos | string | текст |
| bue | bool | булево |
| ptr | * | указатель |
| file | FILE* | файл |

## Сборка компилятора

```bash
./build.sh
# или:
clang++ main.cpp lexer.cpp parser.cpp codegen.cpp \
  -I/path/to/llvm/include \
  $(llvm-config --cxxflags --ldflags --libs) \
  -fexceptions -o sviroxk
```

### Запуск программы

```bash
./sviroxk program.svx > output.ll
llc output.ll -o output.s
clang output.s -o program -lm
./program
```

## Автор

winchestersdc79-hub — создан с нуля на телефоне через Termux
