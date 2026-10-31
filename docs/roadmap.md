# SC — техническая дорожная карта

Ранжирование учитывает текущий C++/LLVM-прототип, существующие тесты и зависимости между работами. Риск: низкий/средний/высокий. Игры и игровой движок не входят в scope.

| Приоритет | Идея | Польза | Что требуется | Сложность / риск |
|---:|---|---|---|---|
| 1 | CLI `--version`, `--help`, стабильные exit codes | Делает компилятор пригодным для скриптов и редакторов | Обработка argv, версия, тесты CLI | Низкая / низкий |
| 2 | CI: build, test, warnings, sanitizers | Ловит регрессии автоматически | GitHub Actions, LLVM setup, стабильные runners | Низкая-средняя / низкий |
| 3 | Conformance suite и EBNF реализованной грамматики | Связывает спецификацию с проверяемым поведением | Каталог позитивных/негативных fixtures, docs/language.md | Средняя / низкий |
| 4 | Shared diagnostics с source excerpt/caret | Улучшает UX и подготавливает LSP | Source manager, spans в токенах/AST, единый reporter | Средняя / средний |
| 5 | Parser error recovery и несколько ошибок за запуск | Ускоряет исправление исходников | Panic-mode sync points, отказ генерации при errors | Средняя / средний |
| 6 | Реестр типов и semantic model вместо строковых типов | Убирает рассогласование между semantic и codegen | Type IDs, promotion/conversion rules, verified lowering | Высокая / высокий |
| 7 | Control-flow анализ: все пути return, definite assignment | Предотвращает скрытые fallthrough и use-before-init | CFG или надёжный структурный flow analysis | Средняя-высокая / средний |
| 8 | Static arrays с точными element types и bounds metadata | Снимает текущие i32-only ограничения | AST array element type, LLVM layout, checked indexing | Высокая / высокий |
| 9 | Dynamic arrays с длиной, capacity и ownership contract | Безопасные коллекции и основа stdlib | Runtime descriptor, realloc/free rules, OOM handling | Высокая / высокий |
| 10 | Чёткий memory model и типизированные указатели | Снижает UAF/double-free и pointer type confusion | Ownership/borrowing или выбранная проще модель, docs/memory.md | Очень высокая / высокий |
| 11 | Настоящий string type с длиной и UTF-8 API | Убирает fixed buffers и C-string hazards | Runtime layout, allocation/errors, string tests | Высокая / высокий |
| 12 | Модули с namespace/use/pub и symbol isolation | Позволяет крупные многомодульные проекты | Per-module AST/symbol tables, import graph, visibility | Высокая / высокий |
| 13 | Безопасная stdlib math/string/io/fs с Result-like ошибками | Базовые приложения без handwritten C runtime calls | Error type/ABI, runtime, module system, per-function tests | Высокая / высокий |
| 14 | Оптимизации LLVM и reproducible benchmarks | Измеримая скорость программ | PassBuilder, optimization levels, benchmark harness | Средняя-высокая / средний |
| 15 | Versioned package/project format и отдельная компиляция | Пакеты и масштабирование сборок | CLI, manifest/lock, module ABI, dependency resolver | Очень высокая / высокий |

## Рекомендуемая последовательность

Сначала стабилизировать CLI, CI и conformance contract. Затем улучшить diagnostics/parser recovery. После этого формализовать type model и control-flow. Только на этой основе безопасно расширять aggregate layout, strings, ownership, modules и библиотеку. LLVM optimization/benchmarks — после корректного IR. Package manager и self-hosting — поздние этапы.

## Текущий подтверждённый baseline

- Компилятор написан на C++, генерирует LLVM IR, сборка требует LLVM toolchain.
- `.svx` содержит короткий и legacy-синтаксис.
- Работают scalar fixed-width aliases, базовый type checking, относительные file imports, source module `math`.
- Тесты запускают примеры, проверяют expected failures, bounds trap статических массивов и sanitizers вручную.
- Нет version/help CLI, formal conformance spec, namespace imports, типизированного runtime массива, полноценной памяти-модели и зрелой stdlib.
