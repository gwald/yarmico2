@echo off

set hr=%time:~0,2%

if "%hr:~0,1%" equ " " set hr=0%hr:~1,1%
set BackUp=yarmico2-Yaroze-%date:~-4,4%%date:~-10,2%%date:~-7,2%_%hr%%time:~3,2%%time:~6,2%_backup.7z

echo %BackUp%

"C:\Program Files\7-Zip\7z.exe"  a  %BackUp%  "C:\Documents and Settings\NetYaroze\Desktop\NY-demos\yarmico2"

xcopy %BackUp% Z:\scrolla-backup\

rm -f %BackUp%


 