@echo off
REM Build script for engine
SetLocal EnableDelayedExpansion

REM Paths are local to root directory
set build_path=./bin/debug/
set src_path=./engine/src/

REM TODO: figure out how to compile static library

set ignoreWarns=-Wno-writable-strings

REM -Werror
set compiler_flags=-g -shared -Wvarargs -Wall %ignoreWarns%
set include_flags=-I%VULKAN_SDK%/Include
set linker_flags=-luser32 -lvulkan-1 -L%VULKAN_SDK%/Lib
set defines=-D_DEBUG -DPXEXPORT -D_CRT_SECURE_NO_WARNINGS

if not exist %build_path% mkdir %build_path%

echo Building Engine: %defines%
clang %src_path%/platform/win32_phoenix.cpp -o %build_path%phoenix.dll %compiler_flags% %defines% %include_flags%

REM For non debug copilation
REM unset -g flag (profiler and debug information flag)
