#!/bin/bash
# Build everything

set -e  # Exit immediately on error

echo "Building everything..."

pushd engine > /dev/null
source build.sh
popd > /dev/null

pushd testbed > /dev/null
source build.sh
popd > /dev/null

echo "[Half life scientist]: Everything seems to be in order..."
echo "All assemblies built successfully."