#!/bin/bash
# Сборка и запуск: ./run.sh hello.svx
set -e
./build.sh
OUT="${1:-hello.svx}"
BASE=$(basename "$OUT" .svx)
./sviroxk "$OUT" > "$BASE.ll"
llc "$BASE.ll" -o "$BASE.s"
clang "$BASE.s" -o "$BASE" -lm
./"$BASE"
