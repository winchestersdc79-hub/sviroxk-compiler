# Журнал циклов разработки SC

## Цикл 1 — audit/build hygiene baseline
- Базовая ревизия: `0963c1b`; исходная рабочая копия была чистой.
- `./build.sh`: успешно.
- `./test.sh`: успешно; исходный suite компилировал и запускал все зарегистрированные примеры и проверял отрицательные случаи.
- Инвентарь: C++ frontend/backend (`lexer.cpp`, `parser.cpp`, `semantic.cpp`, `codegen.cpp`), LLVM IR generation, source imports by relative path, один source stdlib-модуль `math`; нет CI, системного memory model или полной diagnostics infrastructure.
- Подтверждённые предупреждения `-Wall -Wextra`: precedence в `semantic.cpp::compatible` и unused `fcloseType` в `codegen.cpp`. Исправлены в этом цикле.
- ASan/UBSan с LeakSanitizer нашёл утечки AST `Node*`, создаваемых через `new` в parser. Добавлен arena ownership на время parse/compile и regression `examples/array_write.svx`.
- Сборка `-Wall -Wextra -Werror`: прошла без предупреждений.
- Полный `./test.sh` под `-fsanitize=address,undefined`, `ASAN_OPTIONS=detect_leaks=1`: прошёл.
- Следующее: завершить cleanup git-generated artifacts, затем CLI version/help, спецификацию реализованного синтаксиса и CI. Продолжить аудит UB/краевых случаев.

## Цикл 2 — продолжается
- Baseline для изменений цикла: `a0abc61` и чистый Git status.
- Задача: регулярно прогонять строгую и sanitizer сборку; исправления оформлять малыми отдельными коммитами.
