REM Script to build tests and engine
@echo off

echo "Building everthing..."

call ./engine/build_engine.bat
if %ERRORLEVELS% NEQ 0 (echo Error: %ERRORLEVELS% && exit)

call ./engine/build_testbed.bat
if %ERRORLEVELS% NEQ 0 (echo Error: %ERRORLEVELS% && exit)

echo "Everything built succesfully."
