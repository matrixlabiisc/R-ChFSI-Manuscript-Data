/// @file test_problems.cpp
/// @brief Dense eigenvalue test problem generators with prescribed spectra.

#include "test_problems.hpp"
#include "linalg.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <random>

extern "C" {
void dgeqrf_(const int*, const int*,
             double*, const int*,
             double*, double*, const int*, int*);
void dorgqr_(const int*, const int*, const int*,
             double*, const int*,
             const double*, double*, const int*, int*);
}

namespace rchfsi {

/// Random orthogonal matrix via QR of a Gaussian matrix (seeded).
static Matrix random_orthogonal(int m, unsigned seed)
{
    std::mt19937 rng(seed);
    std::normal_distribution<double> dist(0.0, 1.0);

    Matrix Q(m, m);
    for (int j = 0; j < m; ++j)
        for (int i = 0; i < m; ++i)
            Q(i, j) = dist(rng);

    // QR factorisation via Householder (use LAPACK dgeqrf + dorgqr)

    std::vector<double> tau(m);
    int lwork = -1, info = 0;
    double work_query = 0.0;
    dgeqrf_(&m, &m, Q.data(), &m, tau.data(), &work_query, &lwork, &info);
    lwork = static_cast<int>(work_query);
    std::vector<double> work(lwork);
    dgeqrf_(&m, &m, Q.data(), &m, tau.data(), work.data(), &lwork, &info);

    lwork = -1;
    dorgqr_(&m, &m, &m, Q.data(), &m, tau.data(), &work_query, &lwork, &info);
    lwork = static_cast<int>(work_query);
    work.resize(lwork);
    dorgqr_(&m, &m, &m, Q.data(), &m, tau.data(), work.data(), &lwork, &info);

    return Q;
}

TestProblem build_test_problem(const std::vector<double>& a_eigs,
                                const std::vector<double>& b_eigs,
                                double noise_Binv,
                                bool shared_basis,
                                unsigned seed)
{
    int m = static_cast<int>(a_eigs.size());
    assert((int)b_eigs.size() == m);

    TestProblem prob;
    prob.noise_level = noise_Binv;

    Matrix Qb = random_orthogonal(m, seed);

    // B = Qb * diag(b_eigs) * Qb^T
    Matrix Tmp(m, m);
    for (int j = 0; j < m; ++j)
        for (int i = 0; i < m; ++i)
            Tmp(i, j) = Qb(i, j) * b_eigs[j];
    prob.B.resize(m, m);
    mat_mult(Tmp, Qb, prob.B, 1.0, 0.0, 'N', 'T');
    symmetrise(prob.B);

    // B^{-1} = Qb * diag(1/b_eigs) * Qb^T
    for (int j = 0; j < m; ++j)
        for (int i = 0; i < m; ++i)
            Tmp(i, j) = Qb(i, j) / b_eigs[j];
    prob.Binv.resize(m, m);
    mat_mult(Tmp, Qb, prob.Binv, 1.0, 0.0, 'N', 'T');
    symmetrise(prob.Binv);

    // A
    Matrix Qa;
    if (shared_basis) {
        Qa = Qb;   // same eigenbasis → geneigs = a_j / b_j
    } else {
        Qa = random_orthogonal(m, seed + 1000);
    }

    for (int j = 0; j < m; ++j)
        for (int i = 0; i < m; ++i)
            Tmp(i, j) = Qa(i, j) * a_eigs[j];
    prob.A.resize(m, m);
    mat_mult(Tmp, Qa, prob.A, 1.0, 0.0, 'N', 'T');
    symmetrise(prob.A);

    // ---- Exact solve to get reference eigenpairs ----
    Matrix Acopy = prob.A.clone();
    Matrix Bcopy = prob.B.clone();
    prob.exact_eigvals.resize(m);
    int info = sygv(1, 'V', 'U', m,
                    Acopy.data(), Acopy.ld(),
                    Bcopy.data(), Bcopy.ld(),
                    prob.exact_eigvals.data());
    // Acopy now contains generalized eigenvectors
    prob.exact_eigvecs = std::move(Acopy);

    // ---- Approximate B^{-1}: Dinv = Binv + noise * E, ||E||_2 = 1 ----
    if (noise_Binv > 0.0) {
        std::mt19937 rng(seed + 7777);
        std::normal_distribution<double> dist(0.0, 1.0);
        Matrix E(m, m);
        for (int j = 0; j < m; ++j)
            for (int i = 0; i < m; ++i)
                E(i, j) = dist(rng);
        symmetrise(E);
        // Normalise E so ||E||_2 = 1  (exact spectral norm via SVD)
        {
            Matrix Ecopy = E.clone();
            double enorm = spectral_norm(m, m, Ecopy.data(), Ecopy.ld());
            if (enorm > 0.0) {
                double scale = 1.0 / enorm;
                for (int j = 0; j < m; ++j)
                    for (int i = 0; i < m; ++i)
                        E(i, j) *= scale;
            }
        }

        prob.Dinv.resize(m, m);
        for (int j = 0; j < m; ++j)
            for (int i = 0; i < m; ++i)
                prob.Dinv(i, j) = prob.Binv(i, j) + noise_Binv * E(i, j);
        symmetrise(prob.Dinv);

        // Measure actual ζ = ||Dinv - Binv||
        Matrix Diff(m, m);
        for (int j = 0; j < m; ++j)
            for (int i = 0; i < m; ++i)
                Diff(i, j) = prob.Dinv(i, j) - prob.Binv(i, j);
        prob.noise_level = spectral_norm(m, m, Diff.data(), Diff.ld());
    } else {
        prob.Dinv = prob.Binv.clone();
        prob.noise_level = 0.0;
    }

    return prob;
}

/// Standard eigenvalue problem (B = I), with optional noisy A for filtering.
TestProblem build_standard_problem(const std::vector<double>& eigs,
                                    double noise_A,
                                    unsigned seed)
{
    int m = static_cast<int>(eigs.size());
    TestProblem prob;

    Matrix Q = random_orthogonal(m, seed);

    // A = Q * diag(eigs) * Q^T
    Matrix Tmp(m, m);
    for (int j = 0; j < m; ++j)
        for (int i = 0; i < m; ++i)
            Tmp(i, j) = Q(i, j) * eigs[j];

    prob.A.resize(m, m);
    mat_mult(Tmp, Q, prob.A, 1.0, 0.0, 'N', 'T');
    symmetrise(prob.A);

    // Exact eigenpairs: eigenvalues = eigs (prescribed), eigenvectors = Q
    prob.exact_eigvals.assign(eigs.begin(), eigs.end());
    prob.exact_eigvecs = Q.clone();

    // B = I,  Binv = I,  Dinv = I  (standard problem, exact inverse)
    prob.B.resize(m, m);      prob.B.set_identity();
    prob.Binv.resize(m, m);   prob.Binv.set_identity();
    prob.Dinv.resize(m, m);   prob.Dinv.set_identity();

    // ---- Perturb A:  A_noisy = A + ε E,  ||E||_2 = 1 ----
    if (noise_A > 0.0) {
        std::mt19937 rng(seed + 7777);
        std::normal_distribution<double> dist(0.0, 1.0);
        Matrix E(m, m);
        for (int j = 0; j < m; ++j)
            for (int i = 0; i < m; ++i)
                E(i, j) = dist(rng);
        symmetrise(E);
        // Normalise ||E||_2 = 1
        {
            Matrix Ecopy = E.clone();
            double enorm = spectral_norm(m, m, Ecopy.data(), Ecopy.ld());
            if (enorm > 0.0) {
                double inv = 1.0 / enorm;
                for (int j = 0; j < m; ++j)
                    for (int i = 0; i < m; ++i)
                        E(i, j) *= inv;
            }
        }

        // Build the noisy filter matrix:  A_filter = A + ε E
        // Keep prob.A as the exact matrix for RR and residuals.
        prob.A_filter = prob.A.clone();
        for (int j = 0; j < m; ++j)
            for (int i = 0; i < m; ++i)
                prob.A_filter(i, j) += noise_A * E(i, j);
        symmetrise(prob.A_filter);

        prob.noise_level = noise_A;   // ||ε E||_2 = ε by construction
    } else {
        prob.noise_level = 0.0;
    }

    return prob;
}

} // namespace rchfsi
