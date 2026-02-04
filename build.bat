@echo off
echo Building Linear Algebra GUI Application...
echo.

REM Build web server
echo Building web server...
g++ -I Eigen3 -std=c++17 -o server.exe server.cpp Matrix.cpp -lws2_32 -Wall

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ===================================
    echo Build successful!
    echo ===================================
    echo.
    echo How to use:
    echo.
    echo Option 1: Double-click run.bat (recommended)
    echo   - Starts server automatically
    echo   - Opens GUI in browser
    echo.
    echo Option 2: Run server.exe manually
    echo   - Opens a command window showing server status
    echo   - Look for "SERVER RUNNING SUCCESSFULLY!"
    echo   - Then open index.html in your browser
    echo.
) else (
    echo Build failed!
    pause
)
