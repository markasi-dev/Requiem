#!/bin/sh
# Build script for testbed

cFilenames=$(find . -name "*.c" | tr '\n' ' ')

assembly="testbed"
compilerFlags="-g"
includeFlags="-Isrc -I../engine/src/"
linkerFlags="-L../bin -lengine -Wl,-rpath,\$ORIGIN"
defines="-D_DEBUG -DRQ_IMPORT"

echo "Building $assembly..."
clang $cFilenames $compilerFlags -o ../bin/$assembly \
    $defines $includeFlags $linkerFlags