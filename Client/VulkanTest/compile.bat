@echo off
set GLSLC_PATH="%~dp0..\Dependencies\glslc\glslc.exe"

echo Compiling shaders...

pushd "%~dp0"
for %%f in (shaders\*.vert shaders\*.frag) do (
    echo Compiling %%f...
    %GLSLC_PATH% "%%f" -o "%%f.spv"
    if errorlevel 1 (
        echo Failed to compile %%f!
        popd
        exit /b 1
    )
)
popd
echo Done!