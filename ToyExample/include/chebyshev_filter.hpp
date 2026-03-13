#pragma once
/// @file chebyshev_filter.hpp
/// @brief Chebyshev polynomial filter: Algorithm 2 (ChFSI) and Algorithm 3 (R-ChFSI).

#include "matrix.hpp"

#include <vector>

namespace rchfsi {

/// Parameters defining the Chebyshev filter polynomial C_p on [lambda_cut, lambda_max].
struct FilterParams {
    int    poly_degree;   ///< Chebyshev polynomial degree p
    double lambda_max;    ///< upper bound of the full spectrum
    double lambda_min;    ///< lower bound of the full spectrum (≤ λ_1)
    double lambda_cut;    ///< upper bound of the *wanted* spectrum (≈ λ_{n+1})
};

/// Algorithm 2 (ChFSI): Y = C_p(D^{-1} A) X.
Matrix chfsi_filter(const Matrix& A,
                    const Matrix& Dinv,
                    const Matrix& X,
                    const FilterParams& params);

/// Algorithm 3 (R-ChFSI): residual-based filter, robust to inexact D^{-1}.
Matrix rchfsi_filter(const Matrix& A,
                     const Matrix& B,
                     const Matrix& Dinv,
                     const Matrix& X,
                     const std::vector<double>& Lambda,
                     const FilterParams& params,
                     const Matrix& A_filter = {});

} // namespace rchfsi
