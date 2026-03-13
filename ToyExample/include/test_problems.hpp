#pragma once
/// @file test_problems.hpp
/// @brief Generators for dense eigenvalue problems with controlled noise.
///
/// Constructs A = Q diag(a_i) Q^T with prescribed eigenvalues and
/// a noisy approximate inverse D^{-1} = B^{-1} + zeta * E (||E||_2 = 1).
/// Exact eigenpairs from LAPACK dsygv serve as ground truth.

#include "matrix.hpp"

#include <vector>

namespace rchfsi {

/// A complete test problem with exact and noisy data.
struct TestProblem {
    Matrix              A;          ///< m × m symmetric (exact)
    Matrix              A_filter;   ///< m × m (optional) noisy A used during filtering only
    Matrix              B;          ///< m × m SPD
    Matrix              Binv;       ///< exact B^{-1}
    Matrix              Dinv;       ///< approximate B^{-1} (noisy)
    std::vector<double> exact_eigvals;
    Matrix              exact_eigvecs;  ///< m × m  (all eigenvectors)
    double              noise_level;    ///< ζ = ||Dinv - Binv|| or ε = ||A_filter - A||_2
};

/// Build a test problem with *prescribed* eigenvalues for A and B.
TestProblem build_test_problem(const std::vector<double>& a_eigs,
                               const std::vector<double>& b_eigs,
                               double noise_Binv,
                               bool shared_basis = true,
                               unsigned seed = 42);

/// Quick helper: build a standard eigenvalue problem (B = I).
TestProblem build_standard_problem(const std::vector<double>& eigs,
                                    double noise_matvec,
                                    unsigned seed = 42);

} // namespace rchfsi
