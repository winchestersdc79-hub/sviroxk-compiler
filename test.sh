#!/bin/bash
# Прогон всех примеров: ./test.sh
set -e
cd "$(dirname "$0")"
./build.sh
echo "=== hello.svx ==="
./run.sh hello.svx
echo "=== test.svx ==="
./run.sh test.svx
echo "=== examples/struct.svx ==="
./run.sh examples/struct.svx
echo "=== examples/struct_init.svx ==="
./run.sh examples/struct_init.svx
echo "=== examples/dor_func.svx ==="
./run.sh examples/dor_func.svx
echo "=== examples/full.svx ==="
./run.sh examples/full.svx
echo "Все тесты прошли."
