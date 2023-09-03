@echo off
setlocal

mkdir bin
pushd bin
mkdir debug
mkdir release
popd

set SourceFiles=../../src/main.cpp

set CompileFlags=-I../../include -L../../lib ../../lib/libraylib.a -DPLATFORM_WEB -s USE_GLFW=3 -o main_emcc.html --shell-file=../../include/shell.html
set LinkFlags=

set ProfileCompileFlags=
set ProfileLinkFlags=

if "%1"=="" (
    set Profile=debug
    goto setflags
)
set Profile=%1

:setflags
if %Profile% == debug (
    set ProfileCompileFlags=-DDEBUG
    set LinkFlags=
) else if %Profile% == release (
    set ProfileCompileFlags=-O3
    set ProfileLinkFlags=
) else (
    echo ERROR: You should either specify debug or release as the first argument.
    exit /b 1
)

if not exist ./bin/%Profile% (
    echo ERROR: Directory ./bin/%Profile% does not exist.
    exit /b 1
)

pushd bin
pushd %Profile%
call emcc %SourceFiles% %CompileFlags% %ProfileCompileFlags%
popd
popd

rem pause