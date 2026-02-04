#include <iostream>
#include <fstream>
#include <exception>
#include "Matrix.h"

int main() {
    while (true) {
                    std::cout << "\n==== Linear Algebra Menu ====" << std::endl;
                    std::cout << "1. Matrix Addition" << std::endl;
                    std::cout << "2. Matrix Subtraction" << std::endl;
                    std::cout << "3. Matrix Multiplication" << std::endl;
                    std::cout << "4. RREF (Gauss-Jordan)" << std::endl;
                    std::cout << "5. REF (Gaussian)" << std::endl;
                    std::cout << "6. Determinant" << std::endl;
                    std::cout << "7. Rank (Span)" << std::endl;
                    std::cout << "8. Linear Independence" << std::endl;
                    std::cout << "9. Basis" << std::endl;
                    std::cout << "10. Eigenvalues/Eigenvectors" << std::endl;
                    std::cout << "11. Inverse" << std::endl;
                    std::cout << "12. Orthogonality/Orthonormality" << std::endl;
                    std::cout << "13. Gram-Schmidt" << std::endl;
                    std::cout << "14. Diagonalization" << std::endl;
                    std::cout << "15. SVD" << std::endl;
                    std::cout << "0. Exit" << std::endl;
                    std::cout << "Select an option: ";
                    int choice; std::cin >> choice;
                    if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(10000, '\n'); continue; }
                    if (choice == 0) break;

                    std::cout << "Input matrix by: 1) Manual 2) CSV file? (1/2): ";
                    int input_mode; std::cin >> input_mode;
                    if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(10000, '\n'); continue; }
                    std::cin.ignore(10000, '\n'); // clear newline
                    Matrix A(1,1), B(1,1);
                    bool input_ok = true;
                    if (input_mode == 2) {
                        std::string fname;
                        std::cout << "Enter CSV filename (type the filename only when prompted, not at the PowerShell prompt): ";
                        std::getline(std::cin, fname);
                        std::ifstream testfile(fname);
                        if (!testfile.good()) {
                            std::cout << "File not found. Please check the filename and try again.\n"; input_ok = false;
                        } else {
                            try {
                                A = Matrix::fromCSV(fname);
                            } catch (const std::exception& e) {
                                std::cout << "Error loading file: " << e.what() << std::endl; input_ok = false;
                            }
                        }
                        if (input_ok && (choice == 1 || choice == 2 || choice == 3)) {
                            std::cout << "Enter second matrix CSV filename: ";
                            std::getline(std::cin, fname);
                            std::ifstream testfile2(fname);
                            if (!testfile2.good()) {
                                std::cout << "File not found. Please check the filename and try again.\n"; input_ok = false;
                            } else {
                                try {
                                    B = Matrix::fromCSV(fname);
                                } catch (const std::exception& e) {
                                    std::cout << "Error loading file: " << e.what() << std::endl; input_ok = false;
                                }
                            }
                        }
                    } else {
                        int rows, cols;
                        std::cout << "Enter number of rows: "; std::cin >> rows;
                        if (std::cin.fail() || rows <= 0) { std::cin.clear(); std::cin.ignore(10000, '\n'); input_ok = false; }
                        std::cout << "Enter number of columns: "; std::cin >> cols;
                        if (std::cin.fail() || cols <= 0) { std::cin.clear(); std::cin.ignore(10000, '\n'); input_ok = false; }
                        std::vector<std::vector<double>> data(rows, std::vector<double>(cols));
                        if (input_ok) {
                            std::cout << "Enter matrix values row-wise:" << std::endl;
                            for (int i = 0; i < rows; ++i)
                                for (int j = 0; j < cols; ++j) {
                                    std::cin >> data[i][j];
                                    if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(10000, '\n'); input_ok = false; break; }
                                }
                        }
                        if (input_ok) A = Matrix(data);
                        if (input_ok && (choice == 1 || choice == 2 || choice == 3)) {
                            std::cout << "Enter second matrix values row-wise:" << std::endl;
                            for (int i = 0; i < rows; ++i)
                                for (int j = 0; j < cols; ++j) {
                                    std::cin >> data[i][j];
                                    if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(10000, '\n'); input_ok = false; break; }
                                }
                            if (input_ok) B = Matrix(data);
                        }
                    }
                    if (!input_ok) { std::cout << "Input error or CSV load error. Returning to menu.\n"; continue; }

                    switch (choice) {
                        case 1:
                            std::cout << "A + B =\n"; (A + B).print(); break;
                        case 2:
                            std::cout << "A - B =\n"; (A - B).print(); break;
                        case 3:
                            std::cout << "A * B =\n"; (A * B).print(); break;
                        case 4:
                            std::cout << "RREF(A) =\n"; A.rref().print(); break;
                        case 5:
                            std::cout << "REF(A) =\n"; A.ref().print(); break;
                        case 6:
                            std::cout << "det(A) = " << A.determinant() << std::endl; break;
                        case 7:
                            std::cout << "rank(A) = " << A.rank() << std::endl; break;
                        case 8:
                            std::cout << "Linearly independent: " << (A.isLinearlyIndependent() ? "Yes" : "No") << std::endl; break;
                        case 9:
                            std::cout << "Basis for columns of A:\n"; A.basis().print(); break;
                        case 10: {
                            auto eigvals = A.eigenvalues();
                            std::cout << "Eigenvalues: "; for (auto v : eigvals) std::cout << v << " "; std::cout << std::endl;
                            auto eigvecs = A.eigenvectors();
                            std::cout << "Eigenvectors:\n";
                            for (const auto& vec : eigvecs) {
                                for (double x : vec) std::cout << x << " ";
                                std::cout << std::endl;
                            }
                            break;
                        }
                        case 11:
                            std::cout << "A^-1 =\n"; A.inverse().print(); break;
                        case 12:
                            std::cout << "Orthogonal: " << (A.isOrthogonal() ? "Yes" : "No") << ", Orthonormal: " << (A.isOrthonormal() ? "Yes" : "No") << std::endl; break;
                        case 13:
                            std::cout << "Gram-Schmidt(A) =\n"; A.gramSchmidt().print(); break;
                        case 14: {
                            Matrix P(2,2);
                            Matrix D = A.diagonalize(P);
                            std::cout << "Diagonal matrix D:\n"; D.print();
                            std::cout << "Matrix P (eigenvectors):\n"; P.print();
                            break;
                        }
                        case 15: {
                            auto [U, S, V] = A.svd();
                            std::cout << "U:\n"; U.print();
                            std::cout << "S:\n"; S.print();
                            std::cout << "V:\n"; V.print();
                            break;
                        }
                        default:
                            std::cout << "Invalid option." << std::endl;
                    }
        }
    return 0;
}