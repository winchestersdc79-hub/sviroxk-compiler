# Changelog SC

Формат релизов — Semantic Versioning. До `1.0.0` язык имеет статус экспериментального; совместимость публичного синтаксиса и ABI не гарантируется без отдельной пометки в release notes. Legacy-синтаксис не объявляется deprecated без отдельной реализации warning.

## [0.3.0] — текущая разработка

- Добавлены short-circuit `&&`/`||`, bounds checks статических массивов, source-based math helpers.
- Семантическая проверка фиксированных scalar types, numeric literals и явных numeric casts.
- Текущие известные ограничения перечисляются в `STATUS.md`.
