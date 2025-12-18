@echo off
:menu
cls
color 0B
echo ==========================================
echo           MENU TIEN ICH MYSHELL
echo ==========================================
echo 1. Xem cau hinh mang (ipconfig)
echo 2. Kiem tra ket noi internet (ping google)
echo 3. Thoat
echo ==========================================
set /p chon=Ban chon so may (1-3): 

if "%chon%"=="1" goto mang
if "%chon%"=="2" goto ping
if "%chon%"=="3" goto thoat

echo Lua chon khong hop le!
pause
goto menu

:mang
cls
echo Dang lay thong tin IP...
ipconfig
pause
goto menu

:ping
cls
echo Dang ping Google.com...
ping google.com
pause
goto menu

:thoat
echo Bye bye!
exit