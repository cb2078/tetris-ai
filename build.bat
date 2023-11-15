@echo off
:: suppressing global variable and zero sized array extension
cl /nologo /Z7 /W4 /wd4459 /wd4200 /I"raylib\include" main.c /link /nodefaultlib:libcmt /libpath:"raylib\lib" msvcrt.lib raylib.lib opengl32.lib gdi32.lib winmm.lib kernel32.lib shell32.lib user32.lib
