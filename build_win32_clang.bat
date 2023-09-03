@echo off
setlocal

mkdir bin
pushd bin
mkdir debug
mkdir release
popd

set SourceFiles=../../src/main.cpp

set ClangCompileFlags=-DPLATFORM_WIN32 -I../../include
set ClangLinkFlags=-L../../lib -fuse-ld=lld -Wl,-subsystem:console -nostdlib++

set ClangProfileCompileFlags=
set ClangProfileLinkFlags=

if "%1"=="" (
	set Profile=debug
	goto setflags
)
set Profile=%1

:setflags
if %Profile% == debug (
	set ClangProfileCompileFlags=-g -O0 -DDEBUG
	set ClangProfileLinkFlags=
) else if %Profile% == release (
	set ClangProfileCompileFlags=-O3
	set ClangProfileLinkFlags=
) else (
	echo ERROR: You should either specify debug or release as the first argument.
	exit /b 1
)

if not exist ./bin/%Profile% (
	echo ERROR: Bin directory does not exist. You should either specify debug or release as the first argument.
	exit /b 1
)

pushd bin
pushd %Profile%
call clang++ %ClangCompileFlags% %ClangProfileCompileFlags% %ClangLinkFlags% %ClangProfileLinkFlags% %SourceFiles% -o main_clang.exe
popd
popd

pause