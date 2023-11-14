@echo off
:: suppressing global variable and zero sized array extension
cl /nologo /Z7 /W4 /wd4459 /wd4200 main.c
