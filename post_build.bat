@echo off
REM Post build script that compiles shaders to bin
REM and takes all content from assets folder and puts it into the
REM bin folder
SetLocal EnableDelayedExpansion

set shader_compiler=%VULKAN_SDK%\bin\glslc.exe

set in_assets_path=.\assets\
set in_shader_path=%in_assets_path%shaders\

set out_assets_path=.\bin\debug\assets\
set out_shader_path=%out_assets_path%shaders\

set common_compiler_flags=


REM Make sure these match the code!
set object_shader_name=PHOENIX.ObjectShader


REM Run from root dir!
if not exist %out_shader_path% mkdir %out_shader_path%



echo Compiling shaders...

%VULKAN_SDK%\bin\glslc.exe %common_compiler_flags% -fshader-stage=vert %in_shader_path%%object_shader_name%.vert.glsl -o %out_shader_path%%object_shader_name%.vert.spv

if %errorlevel% neq 0 goto error

%VULKAN_SDK%\bin\glslc.exe %common_compiler_flags% -fshader-stage=frag %in_shader_path%%object_shader_name%.frag.glsl -o %out_shader_path%%object_shader_name%.frag.spv

if %errorlevel% neq 0 goto error



echo Copying assets...
REM echo xcopy %in_assets_path% %out_assets_path% /h /i /c /k /e /r /y
REM xcopy %in_assets_path% %out_assets_path% /h /i /c /k /e /r /y

REM if %errorlevel% neq 0 goto error
goto success

:success
echo Done
goto end

:error
echo.
goto end

:end
echo Finished
exit /b %errorlevel%
