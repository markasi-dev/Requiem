#!/bin/bash
# Build script for engine

cFilenames=$(find . -name "*.c" | tr '\n' ' ')

assembly="engine"
compilerFlags="-g -shared -fPIC -Wvarargs -Wall -Werror"
includeFlags="-Isrc -I$VULKAN_SDK/include"
linkerFlags="-lvulkan -L$VULKAN_SDK/lib"
defines="-D_DEBUG -DRQ_EXPORT"

echo "Building $assembly..."
clang $cFilenames $compilerFlags -o ../bin/lib$assembly.so \
    $defines $includeFlags $linkerFlags