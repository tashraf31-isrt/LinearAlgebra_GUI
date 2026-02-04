#include "Matrix.h"
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/SVD>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <vector>

Matrix::Matrix(size_t r, size_t c) : data(Eigen::MatrixXd::Zero(r, c)) {}

Matrix::Matrix(const std::vector<std::vector<double>> &d) {
  size_t r = d.size();
  size_t c = 0;
  for (const auto &row : d)
    if (row.size() > c)
      c = row.size();

  data = Eigen::MatrixXd::Zero(r, c);
  for (size_t i = 0; i < r; ++i) {
    for (size_t j = 0; j < d[i].size(); ++j) {
      data(i, j) = d[i][j];
    }
  }
}

Matrix::Matrix(const Eigen::MatrixXd &mat) : data(mat) {}

Matrix Matrix::operator+(const Matrix &other) const {
  return Matrix(data + other.data);
}

Matrix Matrix::operator*(const Matrix &other) const {
  return Matrix(data * other.data);
}

Matrix Matrix::transpose() const { return Matrix(data.transpose()); }

void Matrix::print() const { std::cout << data << std::endl; }

Matrix Matrix::rref() const {
  Matrix m = *this;
  size_t lead = 0;
  size_t rowCount = m.data.rows();
  size_t colCount = m.data.cols();
  for (size_t r = 0; r < rowCount; ++r) {
    if (lead >= colCount)
      return m;
    size_t i = r;
    while (i < rowCount && std::abs(m.data(i, lead)) < 1e-10)
      ++i;
    if (i == rowCount) {
      ++lead;
      if (lead == colCount)
        return m;
      --r;
      continue;
    }
    m.data.row(i).swap(m.data.row(r));
    double lv = m.data(r, lead);
    for (size_t j = 0; j < colCount; ++j)
      m.data(r, j) /= lv;
    for (size_t i2 = 0; i2 < rowCount; ++i2) {
      if (i2 != r) {
        double lv2 = m.data(i2, lead);
        for (size_t j = 0; j < colCount; ++j)
          m.data(i2, j) -= lv2 * m.data(r, j);
      }
    }
    ++lead;
  }
  return m;
}

Matrix Matrix::ref() const {
  Matrix m = *this;
  size_t lead = 0;
  size_t rowCount = m.data.rows();
  size_t colCount = m.data.cols();
  for (size_t r = 0; r < rowCount; ++r) {
    if (lead >= colCount)
      return m;
    size_t i = r;
    while (i < rowCount && std::abs(m.data(i, lead)) < 1e-10)
      ++i;
    if (i == rowCount) {
      ++lead;
      --r;
      continue;
    }
    m.data.row(i).swap(m.data.row(r));
    double lv = m.data(r, lead);
    for (size_t j = 0; j < colCount; ++j)
      m.data(r, j) /= lv;
    for (size_t i2 = r + 1; i2 < rowCount; ++i2) {
      double lv2 = m.data(i2, lead);
      for (size_t j = 0; j < colCount; ++j)
        m.data(i2, j) -= lv2 * m.data(r, j);
    }
    ++lead;
  }
  return m;
}

double Matrix::determinant() const {
  if (data.rows() != data.cols())
    throw std::invalid_argument(
        "Determinant is defined only for square matrices.");
  return data.determinant();
}

size_t Matrix::rank() const {
  Matrix r = this->rref();
  size_t rank = 0;
  size_t rowCount = r.data.rows();
  size_t colCount = r.data.cols();
  for (size_t i = 0; i < rowCount; ++i) {
    bool nonzero = false;
    for (size_t j = 0; j < colCount; ++j) {
      if (std::abs(r.data(i, j)) > 1e-10) {
        nonzero = true;
        break;
      }
    }
    if (nonzero)
      ++rank;
  }
  return rank;
}

bool Matrix::isLinearlyIndependent() const {
  return rank() == std::min(data.rows(), data.cols());
}

Matrix Matrix::basis() const {
  Matrix r = this->rref();
  std::vector<size_t> pivot_cols;
  size_t rowCount = r.data.rows();
  size_t colCount = r.data.cols();

  // Find pivot columns: For each row, find the first non-zero column (the
  // pivot)
  for (size_t i = 0; i < rowCount; ++i) {
    for (size_t j = 0; j < colCount; ++j) {
      if (std::abs(r.data(i, j)) > 1e-10) {
        // This is a pivot column (first non-zero in this row)
        pivot_cols.push_back(j);
        break;
      }
    }
  }

  if (pivot_cols.empty())
    return Matrix(0, 0);

  // Return the corresponding columns from the ORIGINAL matrix
  std::vector<std::vector<double>> basis_cols(
      rowCount, std::vector<double>(pivot_cols.size()));
  for (size_t j = 0; j < pivot_cols.size(); ++j) {
    size_t col_idx = pivot_cols[j];
    for (size_t i = 0; i < rowCount; ++i) {
      basis_cols[i][j] = data(i, col_idx);
    }
  }
  return Matrix(basis_cols);
}

std::vector<double> Matrix::eigenvalues() const {
  if (data.rows() != data.cols())
    throw std::invalid_argument("Eigenvalues only for square matrices");
  Eigen::EigenSolver<Eigen::MatrixXd> es(data);
  Eigen::VectorXcd eigvals = es.eigenvalues();
  std::vector<double> result;
  for (int i = 0; i < eigvals.size(); ++i) {
    if (std::abs(eigvals(i).imag()) < 1e-10)
      result.push_back(eigvals(i).real());
    // else skip complex eigenvalues for now
  }
  return result;
}

std::vector<std::vector<double>> Matrix::eigenvectors() const {
  if (data.rows() != data.cols())
    throw std::invalid_argument("Eigenvectors only for square matrices");
  Eigen::EigenSolver<Eigen::MatrixXd> es(data);
  Eigen::MatrixXcd eigvecs = es.eigenvectors();
  std::vector<std::vector<double>> result;
  for (int i = 0; i < eigvecs.cols(); ++i) {
    std::vector<double> v;
    for (int j = 0; j < eigvecs.rows(); ++j) {
      if (std::abs(eigvecs(j, i).imag()) < 1e-10)
        v.push_back(eigvecs(j, i).real());
      else
        v.push_back(0.0); // skip complex part
    }
    result.push_back(v);
  }
  return result;
}

Matrix Matrix::inverse() const {
  if (data.rows() != data.cols())
    throw std::invalid_argument("Inverse only for square matrices");
  Matrix aug(data.rows(), 2 * data.cols());
  // Form [A | I]
  for (size_t i = 0; i < data.rows(); ++i) {
    for (size_t j = 0; j < data.cols(); ++j)
      aug.data(i, j) = data(i, j);
    for (size_t j = 0; j < data.cols(); ++j)
      aug.data(i, j + data.cols()) = (i == j) ? 1.0 : 0.0;
  }
  // RREF
  aug = aug.rref();
  // Check invertibility
  for (size_t i = 0; i < data.rows(); ++i)
    if (std::abs(aug.data(i, i) - 1.0) > 1e-10)
      throw std::runtime_error("Matrix is singular");
  // Extract inverse
  Matrix inv(data.rows(), data.cols());
  for (size_t i = 0; i < data.rows(); ++i)
    for (size_t j = 0; j < data.cols(); ++j)
      inv.data(i, j) = aug.data(i, j + data.cols());
  return inv;
}

bool Matrix::isOrthogonal() const {
  if (data.rows() != data.cols())
    return false;
  Matrix prod = (*this) * this->transpose();
  for (size_t i = 0; i < data.rows(); ++i)
    for (size_t j = 0; j < data.cols(); ++j)
      if (i == j && std::abs(prod.data(i, j) - 1.0) > 1e-10)
        return false;
      else if (i != j && std::abs(prod.data(i, j)) > 1e-10)
        return false;
  return true;
}

bool Matrix::isOrthonormal() const { return isOrthogonal(); }

Matrix Matrix::gramSchmidt() const {
  std::vector<std::vector<double>> ortho;
  for (size_t j = 0; j < data.cols(); ++j) {
    std::vector<double> v(data.rows());
    for (size_t i = 0; i < data.rows(); ++i)
      v[i] = data(i, j);
    for (const auto &u : ortho) {
      double dot = 0, norm2 = 0;
      for (size_t k = 0; k < data.rows(); ++k) {
        dot += v[k] * u[k];
        norm2 += u[k] * u[k];
      }
      if (norm2 > 1e-12) {
        for (size_t k = 0; k < data.rows(); ++k)
          v[k] -= dot / norm2 * u[k];
      }
    }
    ortho.push_back(v);
  }
  Matrix result(data.rows(), data.cols());
  for (size_t j = 0; j < data.cols(); ++j)
    for (size_t i = 0; i < data.rows(); ++i)
      result.data(i, j) = ortho[j][i];
  return result;
}

Matrix Matrix::diagonalize(Matrix &P) const {
  if (data.rows() != data.cols())
    throw std::runtime_error("Diagonalization only for square matrices");
  auto eigvals = eigenvalues();
  auto eigvecs = eigenvectors();
  Eigen::MatrixXd D = Eigen::MatrixXd::Zero(data.rows(), data.cols());
  for (int i = 0; i < D.rows(); ++i)
    D(i, i) = (i < eigvals.size() ? eigvals[i] : 0.0);
  Eigen::MatrixXd Pmat(data.rows(), data.cols());
  for (int i = 0; i < data.rows(); ++i)
    for (int j = 0; j < data.cols(); ++j)
      Pmat(i, j) =
          (j < eigvecs.size() && i < eigvecs[j].size() ? eigvecs[j][i] : 0.0);
  P = Matrix(Pmat);
  return Matrix(D);
}

// SVD for matrices using Eigen's JacobiSVD
std::tuple<Matrix, Matrix, Matrix> Matrix::svd_eigen() const {
  Eigen::JacobiSVD<Eigen::MatrixXd> svd(data, Eigen::ComputeFullU |
                                                  Eigen::ComputeFullV);
  Eigen::MatrixXd U = svd.matrixU();
  Eigen::MatrixXd S = Eigen::MatrixXd::Zero(data.rows(), data.cols());
  Eigen::VectorXd sing_vals = svd.singularValues();
  for (int i = 0; i < std::min(S.rows(), S.cols()); ++i)
    S(i, i) = sing_vals(i);
  Eigen::MatrixXd V = svd.matrixV();
  return {Matrix(U), Matrix(S), Matrix(V.transpose())};
}

// SVD for all matrix sizes using Eigen
std::tuple<Matrix, Matrix, Matrix> Matrix::svd() const { return svd_eigen(); }

// Load matrix from CSV file
Matrix Matrix::fromCSV(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open())
    throw std::runtime_error("Cannot open file");
  std::vector<std::vector<double>> values;
  std::string line;
  while (std::getline(file, line)) {
    std::vector<double> row;
    std::stringstream ss(line);
    std::string cell;
    while (std::getline(ss, cell, ',')) {
      row.push_back(std::stod(cell));
    }
    values.push_back(row);
  }
  size_t r = values.size();
  size_t c = r ? values[0].size() : 0;
  Eigen::MatrixXd mat(r, c);
  for (size_t i = 0; i < r; ++i)
    for (size_t j = 0; j < c; ++j)
      mat(i, j) = values[i][j];
  return Matrix(mat);
}

Matrix Matrix::operator-(const Matrix &other) const {
  return Matrix(data - other.data);
}
