@ECHO OFF
SetLocal EnableDelayedExpansion

SET cFilenames=

REM Collect source files
FOR /R src %%f in (*.c) do (
    SET cFilenames=!cFilenames! "%%f"
)

REM Add glad explicitly
SET cFilenames=!cFilenames! "..\vendor\glad\src\glad.c"

ECHO Files: %cFilenames%

SET assembly=engine

SET compilerFlags=-g -shared -Wall -Werror
SET includeFlags=-Isrc -I..\vendor\glad\include -I%VULKAN_SDK%\Include
SET linkerFlags=-L%VULKAN_SDK%\Lib -lvulkan-1 -luser32
SET defines=-D_DEBUG -DRQ_EXPORT -D_CRT_SECURE_NO_WARNINGS

IF NOT EXIST ..\bin mkdir ..\bin

ECHO Building %assembly%...

clang %cFilenames% %compilerFlags% %defines% %includeFlags% ^
-o ..\bin\%assembly%.dll %linkerFlags%