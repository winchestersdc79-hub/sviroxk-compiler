#!/usr/bin/env bash
# Safe, isolated compiler tests: ./test.sh
set -Eeuo pipefail
cd "$(dirname "$0")"
ulimit -c 0

for tool in timeout llvm-as llc clang; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "Ошибка: не найдена необходимая команда: $tool" >&2
        exit 1
    fi
done

./build.sh
TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' EXIT

sources=(
    "hello.svx"
    "test.svx"
    "examples/struct.svx"
    "examples/struct_init.svx"
    "examples/dor_func.svx"
    "examples/full.svx"
    "examples/modern.svx"
)

for source in "${sources[@]}"; do
    name=${source%.svx}
    work="$TMP_DIR/${name//\//_}"
    mkdir -p "$work"
    echo "=== $source ==="

    if (ulimit -c 0; timeout --signal=TERM 15s ./sc "$source" >"$work/module.ll" 2>"$work/compiler.err"); then
        :
    else
        status=$?
        echo "FAIL: compiler failed or exceeded 15s on $source (status $status)" >&2
        cat "$work/compiler.err" >&2
        exit 1
    fi
    if ! llvm-as "$work/module.ll" -o "$work/module.bc" 2>"$work/llvm.err"; then
        echo "FAIL: invalid LLVM IR from $source" >&2
        cat "$work/llvm.err" >&2
        exit 1
    fi
    if ! llc "$work/module.bc" -o "$work/program.s" 2>"$work/llc.err"; then
        echo "FAIL: LLVM code generation failed for $source" >&2
        cat "$work/llc.err" >&2
        exit 1
    fi
    if ! clang -no-pie "$work/program.s" -o "$work/program" -lm 2>"$work/link.err"; then
        echo "FAIL: native linking failed for $source" >&2
        cat "$work/link.err" >&2
        exit 1
    fi
    if ! timeout --signal=TERM 10s "$work/program" >"$work/output.txt" 2>"$work/runtime.err"; then
        echo "FAIL: program failed or exceeded 10s for $source" >&2
        cat "$work/runtime.err" >&2
        exit 1
    fi
    cat "$work/output.txt"
done

expect_compile_error() {
    local name=$1
    local source=$2
    local work="$TMP_DIR/invalid-$name"
    local status=0
    mkdir -p "$work"
    printf '%s\n' "$source" > "$work/input.svx"

    if (ulimit -c 0; timeout --signal=TERM 10s ./sc "$work/input.svx" >"$work/output.ll" 2>"$work/error.log"); then
        echo "FAIL: invalid program '$name' compiled successfully" >&2
        exit 1
    else
        status=$?
    fi
    if (( status >= 128 )); then
        echo "FAIL: compiler crashed while handling '$name' (status $status)" >&2
        cat "$work/error.log" >&2
        exit 1
    fi
    if ! grep -Eq 'Ошибка [0-9]+:[0-9]+' "$work/error.log"; then
        echo "FAIL: '$name' did not produce a source-location diagnostic" >&2
        cat "$work/error.log" >&2
        exit 1
    fi
    echo "PASS: invalid input '$name' reported an error without crashing"
}

expect_compile_error "missing-expression" 'let value: int = ;'
expect_compile_error "missing-function-parameter" 'fn add(int left,) { return left; }'
expect_compile_error "unexpected-character" 'print(1 ! 2);'
expect_compile_error "unterminated-string" 'print("unfinished);'

echo "All compiler tests passed."
