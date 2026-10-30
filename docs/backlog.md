# Очередь работ SC

Порядок — по приоритету и зависимостям. Не отмечать выполненным без тестов.

## P0 — корректность/безопасность
- [ ] Убрать глобальное состояние parser и сделать AST ownership безопасным при копировании Node (arena пока локальный workaround).
- [ ] Усилить ASan/UBSan на array/pointer/file/memory runtime cases; добавить проверки утечек.
- [ ] Исправить все compiler warnings под `-Wall -Wextra -Werror`.
- [ ] Закрыть UB в numeric literal conversions и signed/unsigned casts.
- [ ] Bounds checks для доступных массивов; определить exit/diagnostic contract.

## P1 — зрелость compiler
- [ ] CLI `--version`, `--help`, `--check`, output modes.
- [ ] Shared diagnostics с кодом, source excerpt/caret, recovery для нескольких parse errors.
- [ ] Проверка definite assignment и return analysis для вложенных control-flow.
- [ ] Корректность LLVM IR при ранних return, вложенных циклах, break/continue.
- [ ] Namespace modules, visibility, stable symbol resolution.

## P2 — библиотека и инструментальная цепочка
- [ ] Math module: расширить безопасные helpers, тестировать каждую API функцию.
- [ ] UTF-8 string representation and safe formatting.
- [ ] `fs`/`io` errors and resource cleanup.
- [ ] Sanitizer CI / warnings CI.
- [ ] EBNF language spec, changelog, compatibility policy.

## P3 — долгосрочно
- [ ] Typed aggregate storage (arrays/structs), safe typed pointers.
- [ ] Memory ownership/resource model with documentation.
- [ ] LLVM optimization pipeline and reproducible benchmarks.
- [ ] Generic type infrastructure, then collections.
- [ ] VS Code tooling / LSP after parser and diagnostics API stabilize.
- [ ] Self-hosting bootstrap.

Не входят в scope: игры и игровой движок.
