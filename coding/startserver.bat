@echo off

start /B ./exe/Release/proxyserver.exe
start /B ./exe/Release/gateserver.exe
start /B ./exe/Release/logicserver.exe
start /B ./exe/Release/cacheserver.exe
start /B ./exe/Release/dbserver.exe
choice /t 1 /d y /n >nul

SETLOCAL EnableDelayedExpansion
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
  set "DEL=%%a"
)

:start


call :ColorText 0a "----------------------------- All server Runing -----------------------------------------"
echo.



goto :eof

:ColorText
<nul set /p ".=%DEL%" > "%~2"
findstr /v /a:%1 /R "^$" "%~2" nul
del "%~2" > nul 2>&1
goto :eof
