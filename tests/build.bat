REM Build script for engine
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files
SET cFilenames=
FOR /R %%f in (*.c) do (SET cFilenames=!cFilenames! %%f)

REM echo "Files: " %cFilenames%

SET assembly=TESTS
SET compilerFlags=-g /Wno-missing-braces
REM -Wall -Werror -save-temps=obj -
SET includeFlags=-Isrc -I../engine/src/
SET linkerFlags=-L../bin/ -lengine.lib
SET defines=-D_DEBUG -DRQ_IMPORT

ECHO "Building %assembly%%..."
clang %cFilenames% %compilerFlags% -o ../bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%