@echo off
REM Build script for tests
SetLocal EnableDelayedExpansion

REM Paths are local to root directory
set build_path=./bin/debug/
set src_path=./testbed/src/
set engine_path=./bin/debug/
set engine_inc_path=./engine/src/

set ignoreWarns=-Wno-writable-strings

set compiler_flags=-g  %ignoreWarns%
REM -Wall -Werror
set include_flags=-I%engine_inc_path%
set linker_flags=-L%engine_path% -lphoenix.lib
set defines=-D_DEBUG -DPXIMPORT

if not exist %build_path% mkdir %build_path%

echo Building Test: %defines%
clang %src_path%main.cpp -o %build_path%testbed.exe %compiler_flags% %defines% %include_flags% %linker_flags%
