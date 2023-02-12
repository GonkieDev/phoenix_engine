@echo off
REM Post build script that compiles shaders to bin
REM and takes all content from assets folder and puts it into the
REM bin folder

set shader_compiler=%VULKAN_SDK%\bin\glslc.exe

set in_assets_path=.\assets\
set in_shader_path=%in_assets_path%shaders\

set out_assets_path=.\bin\debug\assets\
set out_shader_path=%out_assets_path%shaders\

set common_compiler_flags=

REM Run from root dir!
if not exist "%cd%\bin\assets\shaders\" mkdir "%cd%\bin\assets\shaders\"



echo "Compiling shaders..."

%shaderCompiler% %common_compiler_flags% -fshader-stage=vert %in_shader_path%Phoenix_ObjectShader.vert.glsl -o %out_shader_path%Phoenix_ObjectShader.vert.spv

if %errorlevel% neq 0 goto error

%shaderCompiler% %common_compiler_flags% -fshader-stage=frag %in_shader_path%Phoenix_ObjectShader.frag.glsl -o %out_shader_path%Phoenix_ObjectShader.frag.spv

if %errorlevel% neq 0 goto error



echo "Copying assets..."
echo xcopy %in_assets_path% %out_assets_path% /h /i /c /k /e /r /y
xcopy %in_assets_path% %out_assets_path% /h /i /c /k /e /r /y

if %errorlevel% neq 0 goto error
goto success

:success
echo "Done"
goto end

:error
echo.
goto end

:end
echo Finished
exit /b %errorlevel%
