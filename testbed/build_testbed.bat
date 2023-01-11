@echo off
REM Build script for tests
SetLocal EnableDelayedExpansion

REM Paths are local to root directory
set build_path=./bin/debug/
set src_path=./testbed/src/
set engine_path=%build_path%
set engine_inc_path=./engine/src/

set ignoreWarns=-Wno-writable-strings

set compiler_flags=-g -m64 "-Wl,/subsystem:windows" %ignoreWarns%
REM -Wall -Werror
set include_flags=-I%engine_inc_path%
set linker_flags=-luser32 -lvulkan-1 -L%VULKAN_SDK%/Lib -L%engine_path% -lphoenix.lib
set defines=-D_DEBUG

if not exist %build_path% mkdir %build_path%

echo Building Test: %defines%
clang %src_path%main.cpp -o %build_path%testbed.exe %compiler_flags% %defines% %include_flags% %linker_flags%
