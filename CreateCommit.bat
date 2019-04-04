@echo off
set /p version="[COMMIT  VER] "
set /p descrip="[DESCRIPTION] "
set name=%version% - %descrip%

call git reset .
call git add *
call git status
echo %name%

set /p docommit="[CONTINUE?YN] "

if /i not "%docommit%"=="y" exit
call git commit -m "%name%"
call git push origin master

pause