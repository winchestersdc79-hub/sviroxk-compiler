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

## Цикл 2 — memory ownership и scalar bounds
- Baseline: `a0abc61`; обычные build/test прошли.
- LeakSanitizer воспроизвёл утечку `Node*` в parser, включая binary-expression copies. Перевёл parser allocations на arena, владеющую узлами весь parse/compile lifetime.
- Добавлен `examples/array_write.svx` для ранее непроверенного array element assignment/read.
- `./test.sh`: все positive/negative cases прошли.
- `CXXFLAGS='-Wall -Wextra -Werror' ./build.sh`: прошёл без предупреждений.
- `CXXFLAGS='-Wall -Wextra -Werror -fsanitize=address,undefined -fno-omit-frame-pointer' ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=print_stacktrace=1 ./test.sh`: прошёл без ASan/UBSan/LeakSanitizer ошибок.
- Исправлено: unused `fcloseType`, warning grouping, AST leaks. Ограничение: arena пока глобальная и parser state global; глубоко-copying Node с raw children остаётся архитектурным долгом.
- Добавлена семантическая валидация numeric literals в выражениях; integer/floating overflow и suffix garbage теперь получают обычную semantic diagnostic до LLVM codegen. Тесты `print(9223372036854775808)`, `print(1e9999)`, `print(123abc)` проходят как ожидаемые ошибки.
- После добавления range tests полный обычный suite прошёл; требуется повторить санитайзеры на финальном состоянии этого исправления.
- Санитайзеры повторены после range validation: обычный suite и ASan/UBSan + LeakSanitizer прошли.
- Исправлено: `math_abs(INT_MIN)` ранее вычислял `0 - value` в i32 и вызывал signed overflow. Теперь вход `int` расширяется до `long` до отрицания, результат API — `long`.
- Сигнатуры `math_min`, `math_max`, `math_is_even` приведены к `long`, чтобы принимать результат `math_abs` без сужения.
- Добавлен `examples/stdlib_abs_min.svx` с ожидаемым результатом `2147483648` и негативный тест на аргумент за пределами `int`.
- Обычный `./test.sh`: прошёл; полный `-Wall -Wextra -Werror -fsanitize=address,undefined`, LeakSanitizer `detect_leaks=1`: прошёл.
- Следующий приоритет: улучшить parser recovery / устранить глобальное состояние, либо сделать целочисленные касты checked runtime operation.

## Цикл 3 — безопасная замена строк
- Воспроизведён дефект legacy `rep`: фиксированный 512-byte stack buffer переполнялся на длинной строке; при совпадении результат был исходной строкой с replacement, дописанным в конец, а не заменой фрагмента.
- Backend переписан: вычисляет длины, ищет первое совпадение, checked-add размера, heap alloc точного объёма, memcpy prefix/replacement/tail; отсутствие совпадения возвращает отдельную heap copy. Результат имеет тип `ptr`; caller должен `del`.
- Семантика проверяет все три аргумента `rep` как `str`; отрицательный test на integer argument.
- Добавлен integration example с заменой в середине строки, отсутствующим needle и строкой больше прежнего лимита; обе возвращённые строки освобождаются.
- Обычный `./test.sh`: прошёл. ASan/UBSan + LeakSanitizer `detect_leaks=1`: прошёл.
- OOM и размерный overflow в текущем generated code завершаются `abort`, не Result/diagnostic; semantics заменяет первое совпадение. Это документировано как остаточное ограничение.
- Следующее: проверить и закоммитить текущую задачу, затем продолжить P0 аудит.

## Цикл 4 — short-circuit логика
- Baseline: `2c9fe7a`; до правки `&&`/`||` строились как eager `and/or`, то есть RHS с эффектами выполнялся всегда.
- Добавлен `toBool` для integer/float truthy conversion и control-flow lowering через условную ветку и PHI: `false && rhs` пропускает RHS, `true || rhs` пропускает RHS.
- Добавлен `examples/short_circuit.svx`, где RHS печатает маркер; тест ожидает только итоговую строку и тем самым подтверждает отсутствие обоих побочных эффектов.
- Обычный `./test.sh`: прошёл. ASan/UBSan с warnings-as-errors и LeakSanitizer: прошёл.
- Случайный сбой проверки оказался дублирующей строкой print в fixture, а не ошибкой CFG; дубликат удалён, stdout сравнивается точно.
- Остаток: результат этой цепочки проверен; общий стресс-тест deeply nested logical CFG остаётся задачей conformance suite.

## Цикл 5 — статические bounds checks
- Подтверждено, что статические array reads/writes ранее не проверяли индекс до inbounds GEP.
- Добавлена проверка отрицательного и `index >= length` для статических массивов с line/column runtime diagnostic и ненулевым exit.
- Позитивный array-write и негативные `-1`/`length` примеры прошли обычный и ASan/UBSan suite.
- Dynamic ptr arrays не хранят runtime length; их границы остаются непроверяемыми и задокументированы как ограничение.
- Коммит: `9d9eb8a`.

## Цикл 6 — текущая работа
- Проверено состояние repo и backlog. Остались незакоммиченные `codegen.cpp`, `semantic.cpp`, `test.sh`, `examples/string_replace.svx` после предыдущего rep fix.
- При очередной сборке выяснилось, что `build.sh` создаёт `sc` без executable permission; sanitizer runner спотыкался на status 126. Исправлено добавлением chmod на бинарник.
- Подтверждены обычная и ASan/UBSan/LeakSanitizer сборки и тесты после исправления тестовой длины expected string.
- Дальше: зафиксировать исправленный rep и build script; затем продолжить P0 audit, не теряя пользовательские изменения.
