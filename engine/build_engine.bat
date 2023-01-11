@echo off
REM Build script for engine
SetLocal EnableDelayedExpansion

REM NOTE: Paths are local to root directory
set build_path=./bin/debug/
set src_path=./engine/src/

set ignoreWarns=-Wno-writable-strings

REM -Werror
set compiler_flags=-g -c -m64 -Wvarargs -Wall %ignoreWarns%
set include_flags=-I%VULKAN_SDK%/Include
set linker_flags=-luser32 -lvulkan-1 -L%VULKAN_SDK%/Lib
set defines=-D_DEBUG -D_CRT_SECURE_NO_WARNINGS

if not exist %build_path% mkdir %build_path%

echo Building Engine: %defines%

REM Comile obj file
clang %src_path%/platform/win32_phoenix.cpp -o %build_path%phoenix.o %compiler_flags% %defines% %include_flags%

REM Lib
llvm-lib %build_path%phoenix.o /out:%build_path%phoenix.lib


REM For non debug copilation
REM unset -g flag (profiler and debug information flag)
