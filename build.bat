@echo off

:: suppress: global variable, zero sized array extension, brackets for precedence
set CF=/nologo /Z7 /W4 /wd4459 /wd4200 /wd4554 /D_CRT_SECURE_NO_WARNINGS

if not exist build mkdir build
pushd build
:: cl %CF% ..\generator.c & generator
cl %CF% /I"..\raylib\include" ..\main.c /link /nodefaultlib:libcmt /libpath:"..\raylib\lib" msvcrt.lib raylib.lib opengl32.lib gdi32.lib winmm.lib kernel32.lib shell32.lib user32.lib
popd
