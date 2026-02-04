#include "Matrix.h"
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>

// Simple JSON response builder
std::string jsonEscape(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == '"') result += "\\\"";
        else if (c == '\\') result += "\\\\";
        else if (c == '\n') result += "\\n";
        else if (c == '\r') result += "\\r";
        else if (c == '\t') result += "\\t";
        else result += c;
    }
    return result;
}

std::string matrixToString(const Matrix& m) {
    std::ostringstream oss;
    oss << m.data;
    return jsonEscape(oss.str());
}

std::string formatResult(const std::string& operation, const std::string& result) {
    return R"({"success": true, "title": ")" + operation + R"(", "result": ")" + result + R"(", "message": "Operation completed successfully"})";
}

std::string errorResponse(const std::string& error) {
    return R"({"success": false, "error": ")" + jsonEscape(error) + R"("})";
}

// Parse command line arguments
int main(int argc, char* argv[]) {
    std::cout << "Linear Algebra Toolkit - Backend Server" << std::endl;
    std::cout << "Open index.html in a web browser to use the GUI" << std::endl;
    std::cout << "Serving on: http://localhost:8080" << std::endl;

    // This is a simple CLI backend - for full web integration, you'd use a library like cpp-httplib
    // For now, we'll create a simple command-line interface that processes operations
    
    std::string command;
    Matrix matrixA(2, 2);
    Matrix matrixB(2, 2);
    
    // Initialize with identity matrices
    std::vector<std::vector<double>> dataA = {{1, 0}, {0, 1}};
    std::vector<std::vector<double>> dataB = {{1, 0}, {0, 1}};
    matrixA = Matrix(dataA);
    matrixB = Matrix(dataB);

    while (true) {
        std::cout << "\nEnter command (addition/subtraction/multiplication/transpose/etc) or 'help' for list:" << std::endl;
        std::getline(std::cin, command);

        if (command == "help") {
            std::cout << "Available operations:\n"
                      << "  addition, subtraction, multiplication, transpose, rref, ref\n"
                      << "  determinant, rank, inverse, trace, independence, basis\n"
                      << "  eigenvalues, eigenvectors, orthogonal, gramschmidt, diagonalize, svd\n"
                      << "  quit\n";
        } else if (command == "quit") {
            break;
        } else if (command == "addition") {
            try {
                Matrix result = matrixA + matrixB;
                result.print();
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "subtraction") {
            try {
                Matrix result = matrixA - matrixB;
                result.print();
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "multiplication") {
            try {
                Matrix result = matrixA * matrixB;
                result.print();
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "transpose") {
            try {
                Matrix result = matrixA.transpose();
                result.print();
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "rref") {
            try {
                Matrix result = matrixA.rref();
                result.print();
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "ref") {
            try {
                Matrix result = matrixA.ref();
                result.print();
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "determinant") {
            try {
                double det = matrixA.determinant();
                std::cout << "Determinant: " << det << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "rank") {
            try {
                int rank = matrixA.rank();
                std::cout << "Rank: " << rank << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "inverse") {
            try {
                Matrix result = matrixA.inverse();
                result.print();
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "independence") {
            try {
                bool indep = matrixA.isLinearlyIndependent();
                std::cout << "Linearly independent: " << (indep ? "Yes" : "No") << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "basis") {
            try {
                Matrix result = matrixA.basis();
                result.print();
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "eigenvalues") {
            try {
                auto eigvals = matrixA.eigenvalues();
                std::cout << "Eigenvalues: ";
                for (double v : eigvals) std::cout << v << " ";
                std::cout << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "eigenvectors") {
            try {
                auto eigvecs = matrixA.eigenvectors();
                std::cout << "Eigenvectors:\n";
                for (const auto& vec : eigvecs) {
                    for (double x : vec) std::cout << x << " ";
                    std::cout << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "orthogonal") {
            try {
                bool ortho = matrixA.isOrthogonal();
                bool orthonorm = matrixA.isOrthonormal();
                std::cout << "Orthogonal: " << (ortho ? "Yes" : "No") << std::endl;
                std::cout << "Orthonormal: " << (orthonorm ? "Yes" : "No") << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "gramschmidt") {
            try {
                Matrix result = matrixA.gramSchmidt();
                result.print();
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "diagonalize") {
            try {
                Matrix P(matrixA.data.rows(), matrixA.data.cols());
                Matrix D = matrixA.diagonalize(P);
                std::cout << "Diagonal Matrix:\n";
                D.print();
                std::cout << "Eigenvector Matrix:\n";
                P.print();
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command == "svd") {
            try {
                auto [U, S, V] = matrixA.svd();
                std::cout << "U:\n";
                U.print();
                std::cout << "S:\n";
                S.print();
                std::cout << "V:\n";
                V.print();
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (command.find("load_a") == 0) {
            std::string filename = command.substr(7);
            try {
                matrixA = Matrix::fromCSV(filename);
                std::cout << "Matrix A loaded from " << filename << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error loading file: " << e.what() << std::endl;
            }
        } else if (command.find("load_b") == 0) {
            std::string filename = command.substr(7);
            try {
                matrixB = Matrix::fromCSV(filename);
                std::cout << "Matrix B loaded from " << filename << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error loading file: " << e.what() << std::endl;
            }
        } else if (!command.empty()) {
            std::cout << "Unknown command. Type 'help' for available operations." << std::endl;
        }
    }

    return 0;
}
