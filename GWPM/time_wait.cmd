==============================================

@echo off
:asd
echo %DATE% %TIME% & netstat | find "CLOSE_WAIT" /c
choice /t 2 /d Y > nul
goto asd

==============================================

