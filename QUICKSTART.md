# Quick Start Guide - Linear Algebra GUI

## 5-Minute Setup

### Step 1: Build the Application
```bash
cd LinearAlgebra_GUI
.\build.bat
```

### Step 2: Choose Your Interface

#### A) Web GUI (Easiest)
1. Open `index.html` in your web browser (Chrome, Firefox, Edge, Safari)
2. Enter matrix values manually
3. Click any operation button
4. View results instantly

#### B) Command-Line Interface
1. Run `backend.exe`
2. Type `help` to see all operations
3. Load matrices or work with defaults
4. Type commands like `addition`, `multiplication`, etc.

## Common Tasks

### Load a Matrix from CSV
```
Web GUI: Click "CSV File" mode, enter filename, click Load
CLI: Type: load_a mymatrix.csv
```

### Perform Matrix Multiplication
```
Web GUI: Enter matrices, click "Multiplication (A * B)"
CLI: Type: multiplication
```

### Find Eigenvalues
```
Web GUI: Click "Eigenvalues (A)"
CLI: Type: eigenvalues
```

### Get RREF
```
Web GUI: Click "RREF (A)"
CLI: Type: rref
```

## CSV File Format

Create a file named `mymatrix.csv`:
```
1,2,3
4,5,6
7,8,9
```

Then load it in the application.

## Tips & Tricks

- ✅ All operations work for any matrix size
- ✅ Use decimal numbers (1.5, 2.7, etc.)
- ✅ Negative numbers are supported
- ✅ Results appear immediately in web GUI
- ✅ Console shows detailed computations in CLI mode

## Troubleshooting

**"File not found" error?**
- Make sure CSV file is in the same directory as the application
- Check the filename spelling

**Computation seems slow?**
- Very large matrices (10000x10000+) will take time
- This is expected for complex operations like SVD

**Web GUI not responding?**
- Try refreshing the page (F5)
- Clear browser cache (Ctrl+Shift+Delete)

## Example Workflow

1. Open `index.html` in browser
2. Change Matrix A rows to 3, cols to 3
3. Enter values like:
   ```
   1 2 3
   0 1 4
   5 0 9
   ```
4. Click "Load Matrix A"
5. Click "RREF (A)" to get reduced row echelon form
6. Results appear instantly!

## Getting Help

- Type `help` in CLI for all available commands
- Check `README.md` for detailed documentation
- All operations show error messages if something fails

---

**Happy computing! 🎓**
