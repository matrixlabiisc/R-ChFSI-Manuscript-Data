#pragma once
/// @file eigensolver.hpp
/// @brief Outer ChFSI / R-ChFSI loop: filter -> Rayleigh-Ritz -> converge.

#include "matrix.hpp"
#include "chebyshev_filter.hpp"

#include <vector>

namespace rchfsi {

/// Per-iteration convergence record.
struct IterRecord {
    int    iter;
    double max_residual;        ///< max_j || A x_j - λ_j B x_j ||
    double subspace_angle;      ///< sin ∠(S^(i)_n, S_{exact,n}) for n wanted eigenpairs
    double delta_norm;          ///< ||Δ̂_p||₂ = ||B^{1/2}(Ỹ_p - Y_p)||₂  (all n_sub columns)
    double eq5_lhs;             ///< |C_p(λ_n)| − |C_p(λ_{n_sub+1})|  (Chebyshev gap; angle terms moved to RHS)
    double eq5_rhs;             ///< ||Δ̂_p||₂ · (sec θ + csc θ)  (filter error × angle factor; θ = entering angle θ^(i−1))
    double eq5_angle;           ///< sin ∠(S^(i)_{n_sub}, S_{exact,n}) — current (post-RR) angle; entering angle for NEXT iter
    bool   eq5_satisfied;       ///< lhs > rhs?
};

/// Which filtering algorithm to use.
enum class FilterMethod { ChFSI, RChFSI };

/// Solver configuration.
struct SolverConfig {
    FilterMethod method   = FilterMethod::RChFSI;  ///< Algorithm 2 (ChFSI) or Algorithm 3 (R-ChFSI)
    FilterParams filter;                     ///< Chebyshev filter bounds and degree
    int          n_wanted = 0;               ///< Number of wanted eigenpairs (smallest eigenvalues)
    int          num_extra = -1;             ///< Extra subspace vectors beyond n_wanted (-1 = auto)
    int          max_iter = 100;             ///< Maximum number of outer filter–RR iterations
    double       tol      = 1e-8;            ///< Convergence tolerance on max residual norm
    unsigned     seed     = 12345;           ///< RNG seed for initial random guess (change to avoid bias)
    bool         track_angle = false;        ///< Compute subspace angles each iteration (needs ref_vecs)
    bool         track_eq5   = false;        ///< Evaluate eq. (5) convergence condition each iteration
    bool         verbose     = true;         ///< Print per-iteration diagnostics to stdout
};

/// Result bundle: converged eigenpairs + convergence history.
struct SolverResult {
    Matrix                   eigvecs;   ///< m × n_wanted converged eigenvectors (B-orthonormal)
    std::vector<double>      eigvals;   ///< n_wanted converged eigenvalues (ascending)
    std::vector<IterRecord>  history;   ///< Per-iteration records [0..iters]; iter 0 = pre-filter
    int                      iters;     ///< Number of filter iterations actually performed
    bool                     converged; ///< True if max residual < tol within max_iter
};

/// Run the eigensolver (Alg. 2 or 3 + Rayleigh-Ritz outer loop).
SolverResult solve(const Matrix& A,
                   const Matrix& B,
                   const Matrix& Dinv,
                   const SolverConfig& cfg,
                   Matrix X0 = {},
                   const Matrix& ref_vecs = {},
                   const std::vector<double>& ref_vals = {},
                   const Matrix& exact_Binv = {},
                   const Matrix& A_filter = {});

// ---- Diagnostic utilities ----

/// max_j || A x_j - lambda_j B x_j ||.
double max_residual_norm(const Matrix& A,
                         const Matrix& B,
                         const Matrix& X,
                         const std::vector<double>& Lambda);

/// sin(theta_max) between column spaces of X and Ref (B-weighted).
double max_principal_angle_sin(const Matrix& X,
                               const Matrix& Ref,
                               const Matrix& B);

/// Evaluate the scaled Chebyshev polynomial C_p(lambda) used by the filter.
double eval_chebyshev(double lambda, const FilterParams& fp);

} // namespace rchfsi
