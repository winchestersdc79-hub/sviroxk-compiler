# Очередь работ SC

Порядок — по приоритету и зависимостям. Не отмечать выполненным без тестов.

## P0 — корректность/безопасность
- [ ] Убрать глобальное состояние parser и сделать AST ownership безопасным при копировании Node (arena пока локальный workaround).
- [~] ASan/UBSan LeakSanitizer проверяет parser arena, heap result `rep` и static array bounds. Динамические pointer/file/memory safety cases требуют runtime metadata.
- [ ] Исправить все compiler warnings под `-Wall -Wextra -Werror`.
- [~] Прямые numeric literals во всех expression contexts проверяются до codegen. Остаются checked casts для вычисляемых значений и точное signed/unsigned conversion semantics.
- [ ] Bounds checks для доступных массивов; определить exit/diagnostic contract.

## P1 — зрелость compiler
- [x] Short-circuit evaluation для `&&`/`||` реализована через LLVM CFG; side-effect regression покрывает пропуск правого операнда.
- [ ] Shared diagnostics с кодом, source excerpt/caret, recovery для нескольких parse errors.
- [ ] Проверка definite assignment и return analysis для вложенных control-flow.
- [ ] Корректность LLVM IR при ранних return, вложенных циклах, break/continue.
- [ ] Namespace modules, visibility, stable symbol resolution.

## P2 — библиотека и инструментальная цепочка
- [~] Math module: `math_abs` безопасно обрабатывает INT_MIN через i64 return; min/max/is_even принимают i64. Остальные math API ещё отсутствуют.
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
