@echo off
REM Script to build tests and engine and runs post build

echo "Building everthing..."

call .\engine\build_engine.bat
if %errorlevels% neq 0 goto error

call .\testbed\build_testbed.bat
if %errorlevels% neq 0 goto error


:success
echo "Everything built succesfully."
goto end

:error
echo.
goto end

:end
echo Finished
exit /b %errorlevel%
