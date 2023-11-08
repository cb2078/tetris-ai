@echo off
REM cl /nologo /Zi /Wall main.c
cl /nologo /Zi /I"raylib\include" tetris.c /link /nodefaultlib:libcmt /libpath:"raylib\lib" msvcrt.lib raylib.lib opengl32.lib gdi32.lib winmm.lib kernel32.lib shell32.lib user32.lib
