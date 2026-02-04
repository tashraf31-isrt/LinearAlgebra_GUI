@echo off
REM Linear Algebra GUI Launcher
REM This script starts the server and opens the GUI in your default browser

cd /d "%~dp0"

echo ============================================
echo Linear Algebra GUI - Launcher
echo ============================================
echo.
echo Checking for server.exe...

REM Check if server.exe exists
if not exist server.exe (
    echo ERROR: server.exe not found!
    echo Please run build.bat first to compile the application
    echo.
    pause
    exit /b 1
)

echo [OK] server.exe found
echo.
echo Starting server in a new window...
echo Keep this window open - the server will run there.
echo.

REM Start server in a new window
start "Linear Algebra GUI - Server" cmd /k "%cd%\server.exe"

REM Wait for server to start
timeout /t 3 /nobreak

REM Open index.html in default browser
echo.
echo Opening GUI in your default browser...
echo.

if exist index.html (
    start index.html
) else (
    echo WARNING: index.html not found!
    echo Please manually open index.html or visit: http://127.0.0.1:8080
)

echo.
echo ============================================
echo Setup complete!
echo ============================================
echo.
echo If the GUI doesn't open automatically:
echo   - Check the "Linear Algebra GUI - Server" window for status
echo   - Open your browser and go to: http://127.0.0.1:8080
echo   - Manually open: index.html
echo.
echo To stop the server:
echo   - Close the "Linear Algebra GUI - Server" window
echo   - Or press Ctrl+C in that window
echo.
pause
