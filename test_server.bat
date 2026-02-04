@echo off
REM Test script to verify the server is working

echo.
echo ============================================
echo Linear Algebra GUI - Server Test
echo ============================================
echo.

cd /d "%~dp0"

echo Testing server startup...
echo.

REM Run server for 5 seconds to see if it starts
timeout /t 1 /nobreak

start "Test Server" cmd /k "server.exe & timeout /t 5"

echo.
echo Server should be running in the new window.
echo.
echo After the server starts (you should see "SERVER RUNNING SUCCESSFULLY!"):
echo.
echo Option 1: Open index.html in your browser
echo Option 2: Visit http://127.0.0.1:8080 in your browser
echo.
echo If server.exe closes immediately without messages:
echo - There may be a missing dependency
echo - Try running from command line: cd LinearAlgebra_GUI && server.exe
echo.
pause
