#!/usr/bin/env bash
# Runs the implemented-language conformance fixtures against the freshly built SC compiler.
set -Eeuo pipefail
cd "$(dirname "$0")"
ulimit -c 0

if [[ ! -x ./sc ]]; then
    echo "Run ./build.sh first; ./sc is missing or not executable." >&2
    exit 1
fi
for tool in timeout llvm-as llc clang; do
    command -v "$tool" >/dev/null 2>&1 || { echo "Missing required command: $tool" >&2; exit 1; }
done
TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' EXIT

run_valid() {
    local name=$1 expected=$2 work="$TMP_DIR/$1"
    mkdir -p "$work"
    if ! timeout --signal=TERM 10s ./sc "docs/conformance/$name.svx" >"$work/module.ll" 2>"$work/compiler.err"; then
        echo "FAIL conformance/$name: compile failed" >&2
        cat "$work/compiler.err" >&2
        exit 1
    fi
    llvm-as "$work/module.ll" -o "$work/module.bc"
    llc "$work/module.bc" -o "$work/program.s"
    clang -no-pie "$work/program.s" -o "$work/program" -lm
    timeout --signal=TERM 10s "$work/program" >"$work/output"
    if [[ "$(cat "$work/output")" != "$expected" ]]; then
        echo "FAIL conformance/$name: unexpected output" >&2
        cat "$work/output" >&2
        exit 1
    fi
    echo "PASS conformance/$name"
}

run_invalid() {
    local name=$1 work="$TMP_DIR/$1" status=0
    mkdir -p "$work"
    if (ulimit -c 0; timeout --signal=TERM 10s ./sc "docs/conformance/$name.svx" >"$work/output.ll" 2>"$work/error"); then
        echo "FAIL conformance/$name: invalid source compiled" >&2
        exit 1
    else
        status=$?
    fi
    if (( status >= 128 )) || ! grep -Eq '^[^:]+:[0-9]+:[0-9]+: error\[' "$work/error"; then
        echo "FAIL conformance/$name: missing controlled source diagnostic" >&2
        cat "$work/error" >&2
        exit 1
    fi
    echo "PASS conformance/$name (expected error)"
}

run_valid basic 42
run_valid operators 7
run_invalid invalid-type
echo "SC conformance fixtures passed."
