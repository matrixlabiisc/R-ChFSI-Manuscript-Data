#pragma once
/// @file matrix.hpp
/// @brief Lightweight column-major dense matrix owning its storage.

#include "linalg.hpp"

#include <algorithm>
#include <cassert>
#include <vector>

namespace rchfsi {

/// Column-major dense matrix of doubles, owning its storage.
class Matrix {
public:
    /// Default-construct an empty (0 × 0) matrix.
    Matrix() = default;

    /// Construct an m × n matrix, zero-initialised.
    Matrix(int rows, int cols)
        : m_(rows), n_(cols), data_(static_cast<size_t>(rows) * cols, 0.0) {}

    int    rows()  const { return m_; }
    int    cols()  const { return n_; }
    int    ld()    const { return m_; }
    double*       data()       { return data_.data(); }
    const double* data() const { return data_.data(); }

    double& operator()(int i, int j)       { return data_[j * (size_t)m_ + i]; }
    double  operator()(int i, int j) const { return data_[j * (size_t)m_ + i]; }

    void zero()     { std::fill(data_.begin(), data_.end(), 0.0); }

    void set_identity() {
        zero();
        int mn = std::min(m_, n_);
        for (int i = 0; i < mn; ++i) (*this)(i, i) = 1.0;
    }

    Matrix clone() const {
        Matrix c(m_, n_);
        c.data_ = data_;
        return c;
    }

    void resize(int rows, int cols) {
        m_ = rows; n_ = cols;
        data_.resize(static_cast<size_t>(rows) * cols);
    }

    double*       col(int j)       { return data_.data() + j * (size_t)m_; }
    const double* col(int j) const { return data_.data() + j * (size_t)m_; }

private:
    int m_ = 0, n_ = 0;
    std::vector<double> data_;
};

/// C = alpha * op(A) * op(B) + beta * C.
inline void mat_mult(const Matrix& A, const Matrix& B, Matrix& C,
                     double alpha = 1.0, double beta = 0.0,
                     char tA = 'N', char tB = 'N')
{
    int m = (tA == 'N') ? A.rows() : A.cols();
    int n = (tB == 'N') ? B.cols() : B.rows();
    int k = (tA == 'N') ? A.cols() : A.rows();
    gemm(tA, tB, m, n, k, alpha,
         A.data(), A.ld(), B.data(), B.ld(),
         beta, C.data(), C.ld());
}

/// Symmetrise: A := (A + A^T) / 2
inline void symmetrise(Matrix& A) {
    assert(A.rows() == A.cols());
    int n = A.rows();
    for (int j = 0; j < n; ++j)
        for (int i = j + 1; i < n; ++i) {
            double avg = 0.5 * (A(i, j) + A(j, i));
            A(i, j) = avg;
            A(j, i) = avg;
        }
}

} // namespace rchfsi
