# Setup & Troubleshooting Guide

## Quick Start

### Method 1: Automatic (Easiest) ⭐
1. **Double-click `run.bat`** in the LinearAlgebra_GUI folder
2. A command window opens showing server status
3. Your browser automatically opens the GUI
4. Start using the application!

### Method 2: Manual Server Start
1. **Double-click `server.exe`** (NOT recommended - window closes too fast)
2. Instead, open Command Prompt and run:
   ```
   cd d:\Programming\LinearAlgebra_GUI
   server.exe
   ```
3. Wait for "SERVER RUNNING SUCCESSFULLY!" message
4. Open `index.html` in your browser or visit `http://127.0.0.1:8080`

## What Should Happen

### When server.exe starts correctly:
```
============================================
Linear Algebra GUI - HTTP Server
============================================

[*] Initializing Winsock...
[OK] Winsock initialized

[*] Creating socket...
[OK] Socket created

[*] Binding socket to port 8080...
[OK] Socket bound to 127.0.0.1:8080

[*] Starting to listen for connections...
[OK] Listening started

============================================
SERVER RUNNING SUCCESSFULLY!
============================================
[+] Web Server: http://127.0.0.1:8080
[+] GUI File: index.html
[+] Status: Waiting for connections...

To stop the server, press Ctrl+C
```

### When you see this, the server is ready!

## Troubleshooting

### Issue: server.exe closes instantly
**Possible Causes:**
1. Port 8080 is already in use
2. Winsock error (rare)
3. Missing dependencies

**Solutions:**
- Check if another application is using port 8080
- Try running `netstat -ano | findstr :8080` in Command Prompt
- Rebuild using `build.bat`

### Issue: "failed to fetch" in browser
**This means:**
- Server is not running
- Browser can't connect to `http://127.0.0.1:8080`

**Solutions:**
1. Make sure server.exe is running in a command window
2. Watch for "SERVER RUNNING SUCCESSFULLY!" message
3. Try accessing `http://127.0.0.1:8080` directly in browser
4. Check if Windows Firewall is blocking the port

### Issue: Port 8080 already in use
**Solution:**
- Open Command Prompt as Administrator
- Run: `netstat -ano | findstr :8080`
- Note the PID of the process using port 8080
- Kill it with: `taskkill /PID <PID> /F`
- Try again

### Issue: "Cannot find Eigen3" error
**Solution:**
- Make sure Eigen3 folder exists in LinearAlgebra_GUI
- Run `build.bat` to rebuild everything

## File Structure
```
LinearAlgebra_GUI/
├── server.exe              ← Main server (run this)
├── run.bat                 ← Auto-start script (double-click this!)
├── test_server.bat         ← Test if server works
├── build.bat               ← Rebuild from source
├── index.html              ← GUI interface
├── server.cpp              ← Server source code
├── Matrix.cpp/Matrix.h     ← Linear algebra library
├── Eigen3/                 ← Math library
└── README.md
```

## Recommended Workflow

1. **First Time:**
   ```
   Double-click build.bat          (Compile the application)
   Double-click run.bat            (Start server and GUI)
   ```

2. **Subsequent Times:**
   ```
   Double-click run.bat            (Opens everything)
   ```

3. **If Server Crashes:**
   ```
   Open Command Prompt
   cd d:\Programming\LinearAlgebra_GUI
   server.exe
   (Check for error messages)
   ```

## Using the GUI

### Input Modes:
- **Manual**: Enter numbers in the grid
- **CSV**: Load matrices from comma-separated files

### Operations Available:
- Basic: Add, Subtract, Multiply, Transpose
- Decompositions: RREF, REF, SVD
- Analysis: Determinant, Rank, Eigenvalues/vectors
- Advanced: Diagonalization, Gram-Schmidt, Basis

### Example Workflow:
1. Set Matrix A dimensions (2x2)
2. Enter values: 1, 2, 3, 4 (in order)
3. Click "Load Matrix A"
4. Click any operation button
5. See results instantly!

## Support

If you still have issues:
1. Try Method 2 (Manual Server Start) to see actual error messages
2. Check that Eigen3 folder exists and has files
3. Verify index.html is in the same folder as server.exe
4. Try rebuilding with: `build.bat`

---

**The GUI should be fully functional once server.exe shows "SERVER RUNNING SUCCESSFULLY!"**
