#!/bin/bash
# Build everything

set -e  # Exit immediately on error

echo "Building everything..."

# pushd engine > /dev/null
# source build.sh
# popd > /dev/null
# 
# pushd testbed > /dev/null
# source build.sh
# popd > /dev/null

make -f Makefile.engine.linux.mak all
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && Exit
f1

make -f Makefile.testbed.linux.mak all
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && Exit
f1

make -f Makefile.tests.linux.mak all
ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "Error:"$ERRORLEVEL && Exit
f1

echo "[Half life scientist]: Everything... seems to be in order!"
echo "All assemblies built successfully."