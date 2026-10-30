#!/usr/bin/env bash
set -Eeuo pipefail
cd "$(dirname "$0")"

if ! command -v llvm-config >/dev/null 2>&1; then
    echo "Ошибка: llvm-config не найден. Установите LLVM и добавьте его в PATH." >&2
    exit 1
fi

BUILD_DIR=$(mktemp -d)
trap 'rm -rf "$BUILD_DIR"' EXIT
CXX=${CXX:-clang++}
LLVM_CXXFLAGS=$(llvm-config --cxxflags)
LLVM_LDFLAGS=$(llvm-config --ldflags --libs)

for source in main lexer parser semantic codegen; do
    "$CXX" -c "$source.cpp" $LLVM_CXXFLAGS -fexceptions -o "$BUILD_DIR/$source.o"
done

"$CXX" "$BUILD_DIR/main.o" "$BUILD_DIR/lexer.o" \
    "$BUILD_DIR/parser.o" "$BUILD_DIR/semantic.o" "$BUILD_DIR/codegen.o" \
    $LLVM_LDFLAGS -fexceptions -o sc
chmod +x run.sh build.sh test.sh 2>/dev/null || true
echo "Сборка SC завершена"
