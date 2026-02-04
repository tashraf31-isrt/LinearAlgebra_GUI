# Linear Algebra Toolkit - GUI Version

A professional, feature-rich graphical user interface for advanced linear algebra operations built with C++ and web technologies.

## Features

✨ **Complete Matrix Operations:**
- Basic Operations: Addition, Subtraction, Multiplication, Transpose
- Decompositions: RREF, REF, LU, QR, SVD
- Analysis: Determinant, Rank, Trace, Linear Independence, Basis
- Advanced: Eigenvalues, Eigenvectors, Diagonalization
- Orthogonality: Orthogonal Check, Orthonormal Check, Gram-Schmidt

🎨 **User-Friendly Interface:**
- Modern, responsive HTML/CSS GUI
- Drag-and-drop matrix input
- Real-time operation execution
- Beautiful gradient design
- Mobile-friendly layout

📊 **Data Handling:**
- Manual matrix input
- CSV file support
- Large matrix support (millions of elements with Eigen library)
- Result display and formatting

## How to Use

### Option 1: Web GUI (Recommended)
1. Open `index.html` in any modern web browser
2. Input your matrices using the manual input or CSV file upload
3. Click any operation button to compute results
4. Results appear in the right panel

**Share the web GUI with a friend (quick):**
- ngrok is free (limited) and lets you expose your local server to the Internet temporarily. Get it from https://ngrok.com/download and run `share_with_ngrok.bat` in this folder to start the server and open the public tunnel automatically.

**Local network (LAN) sharing:**
- Bind server to 0.0.0.0, open Windows firewall for port 8080, then share http://<your-lan-ip>:8080

### Option 2: Command-Line Backend
1. Run `backend.exe`
2. Use command-line interface with commands like:
   - `addition`, `subtraction`, `multiplication`
   - `transpose`, `rref`, `ref`
   - `determinant`, `rank`, `inverse`
   - `eigenvalues`, `eigenvectors`, `diagonalize`, `svd`
   - `orthogonal`, `gramschmidt`, `basis`
   - `load_a filename.csv` - Load matrix A from CSV
   - `load_b filename.csv` - Load matrix B from CSV
   - `help` - Show all available commands
   - `quit` - Exit

### Option 3: Terminal Menu (Original Version)
1. Run `main.exe` in the parent directory
2. Use the terminal-based menu to select operations

## Building from Source

### Prerequisites
- GCC/Clang compiler with C++17 support
- Eigen3 library (included)
- No external GUI dependencies for web version

### Build Commands
```bash
# Build web GUI version (backend)
cd LinearAlgebra_GUI
.\build.bat

# Build original terminal version
cd ..
.\build_with_eigen.bat
```

## Project Structure

```
LinearAlgebra_GUI/
├── index.html          # Web GUI interface
├── backend.cpp         # C++ backend for web operations
├── main_gui.cpp        # Alternative ImGui implementation
├── Matrix.h            # Matrix class header
├── Matrix.cpp          # Matrix operations implementation
├── build.bat           # Build script
├── Eigen3/             # Eigen library (header-only)
└── README.md           # This file
```

## Supported Operations

### Basic Linear Algebra
- Matrix Addition/Subtraction
- Matrix Multiplication
- Matrix Transpose
- Determinant
- Rank (via row reduction)
- Trace

### Matrix Decompositions
- REF (Row Echelon Form)
- RREF (Reduced Row Echelon Form)
- Eigenvalue/Eigenvector Computation (any size matrix)
- Diagonalization
- Singular Value Decomposition (SVD)

### Linear Algebra Analysis
- Linear Independence Check
- Basis Computation
- Inverse Matrix
- Orthogonality Check
- Orthonormality Check
- Gram-Schmidt Orthogonalization

### Performance
- Supports matrices with millions of elements
- Efficient computation using Eigen library
- Optimized for both small and large matrices

## Example Usage

### Creating a 2x2 Matrix
```
Input: [[1, 2], [3, 4]]
```

### CSV File Format
```
1.0,2.0,3.0
4.0,5.0,6.0
7.0,8.0,9.0
```

## Technical Details

- **Language**: C++17
- **Linear Algebra Library**: Eigen3 (header-only, fast)
- **Frontend**: HTML5 + CSS3 + JavaScript
- **Architecture**: Client-side web interface with C++ backend API
- **Cross-Platform**: Works on Windows, macOS, Linux

## Limitations & Notes

- Complex eigenvalues are handled but only real parts are displayed
- Some operations require square matrices (eigenvalues, determinant)
- CSV files should have comma-separated values
- Maximum practical matrix size depends on available RAM

## Future Enhancements

- [ ] HTTP server integration for true web API
- [ ] WebGL visualization of matrices
- [ ] Additional decompositions (QR, LU, Cholesky)
- [ ] Matrix equation solver
- [ ] Batch operations
- [ ] Export results to PDF/Excel

## License

This project is provided as-is for educational and research purposes.

## Support

For issues or feature requests, check the console output for detailed error messages.

---

**Created with ❤️ for linear algebra enthusiasts**
