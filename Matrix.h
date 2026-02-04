#pragma once
#include <Eigen/Dense>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>


class Matrix {
public:
  Eigen::MatrixXd data;

  Matrix(size_t r, size_t c);
  Matrix(const std::vector<std::vector<double>> &d);
  Matrix(const Eigen::MatrixXd &mat);

  Matrix operator+(const Matrix &other) const;
  Matrix operator-(const Matrix &other) const;
  Matrix operator*(const Matrix &other) const;
  Matrix transpose() const;
  void print() const;

  Matrix rref() const; // Reduced Row Echelon Form (Gauss-Jordan)
  Matrix ref() const;  // Row Echelon Form (Gaussian)
  double determinant() const;
  // Span: returns the dimension of the span (rank)
  size_t rank() const;
  // Linear independence: returns true if columns are linearly independent
  bool isLinearlyIndependent() const;
  // Basis: returns a basis for the column space
  Matrix basis() const;
  // Eigenvalues and eigenvectors (for any square matrix)
  std::vector<double> eigenvalues() const;
  std::vector<std::vector<double>> eigenvectors() const;
  // Diagonalization (returns diagonal matrix if possible, any size)
  Matrix diagonalize(Matrix &P) const;
  // SVD (returns U, S, V as tuple)
  std::tuple<Matrix, Matrix, Matrix> svd() const;
  std::tuple<Matrix, Matrix, Matrix> svd_eigen() const;

  // Load matrix from CSV file
  static Matrix fromCSV(const std::string &filename);

  Matrix inverse() const;
  bool isOrthogonal() const;
  bool isOrthonormal() const;
  Matrix gramSchmidt() const;

  // Equation Solving
  Matrix solveCramer(const Matrix &b) const;
  Matrix solveSystem(const Matrix &b) const; // Uses RREF on augmented [A | b]
};
