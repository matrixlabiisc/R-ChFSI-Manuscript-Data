/// @file eigensolver.cpp
/// @brief Solver loop (filter -> Rayleigh-Ritz -> converge) and diagnostics.

#include "eigensolver.hpp"
#include "linalg.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>
#include <random>

namespace rchfsi {

// ---- Residual norms r_j = || A x_j - lambda_j B x_j || ----
static std::vector<double> residual_norms(const Matrix& A,
                                    const Matrix& B,
                                    const Matrix& X,
                                    const std::vector<double>& Lambda)
{
    int m  = X.rows();
    int nv = X.cols();
    assert((int)Lambda.size() == nv);

    Matrix R(m, nv);
    mat_mult(A, X, R);

    // tmp = B * X
    Matrix tmp(m, nv);
    mat_mult(B, X, tmp);

    // R -= tmp * diag(Lambda)
    for (int j = 0; j < nv; ++j)
        for (int i = 0; i < m; ++i)
            R(i, j) -= tmp(i, j) * Lambda[j];

    // per-vector norms
    std::vector<double> nrms(nv);
    for (int j = 0; j < nv; ++j)
        nrms[j] = nrm2(m, R.col(j), 1);

    return nrms;
}

double max_residual_norm(const Matrix& A,
                          const Matrix& B,
                          const Matrix& X,
                          const std::vector<double>& Lambda)
{
    auto nrms = residual_norms(A, B, X, Lambda);
    return *std::max_element(nrms.begin(), nrms.end());
}

// ---- sin(theta_max) via B-orthogonal projection residual ----
//  Avoids catastrophic cancellation in sqrt(1 - cos^2 theta).
double max_principal_angle_sin(const Matrix& X,
                                const Matrix& Ref,
                                const Matrix& B)
{
    int m    = X.rows();
    int nx   = X.cols();
    int nref = Ref.cols();

    // We project the smaller subspace onto the larger one so that
    // all min(nx, nref) principal angles are captured.
    const Matrix& Large = (nx >= nref) ? X : Ref;
    const Matrix& Small = (nx >= nref) ? Ref : X;
    int nlarge = Large.cols();
    int nsmall = Small.cols();

    Matrix BSmall(m, nsmall);
    mat_mult(B, Small, BSmall);
    Matrix G(nlarge, nsmall);
    mat_mult(Large, BSmall, G, 1.0, 0.0, 'T', 'N');

    Matrix LG(m, nsmall);
    mat_mult(Large, G, LG);
    Matrix PSmall(m, nsmall);
    for (int j = 0; j < nsmall; ++j)
        for (int i = 0; i < m; ++i)
            PSmall(i, j) = Small(i, j) - LG(i, j);

    Matrix BPSmall(m, nsmall);
    mat_mult(B, PSmall, BPSmall);
    Matrix H(nsmall, nsmall);
    mat_mult(PSmall, BPSmall, H, 1.0, 0.0, 'T', 'N');
    symmetrise(H);

    std::vector<double> eigs(nsmall);
    syev('N', 'U', nsmall, H.data(), H.ld(), eigs.data());

    double sin2_max = std::max(0.0, eigs.back());
    return std::sqrt(sin2_max);
}

// ---- Scaled Chebyshev polynomial C_p(lambda) using sigma-recurrence ----
double eval_chebyshev(double lambda, const FilterParams& fp)
{
    int p = fp.poly_degree;
    if (p == 0) return 1.0;

    double e = (fp.lambda_max - fp.lambda_cut) / 2.0;
    double c = (fp.lambda_max + fp.lambda_cut) / 2.0;

    double sigma  = e / (fp.lambda_min - c);   // σ₁ = T₀(L(a))/T₁(L(a))
    double sigma1 = sigma;
    double gamma  = 2.0 / sigma1;

    // C_0 = 1,  C_1 = (σ₁/e)(λ − c) = L(λ)/L(λ_min)
    double y_prev = 1.0;
    double y_cur  = (sigma1 / e) * (lambda - c);

    if (p == 1) return y_cur;

    for (int k = 2; k <= p; ++k) {
        double sigma2 = 1.0 / (gamma - sigma);
        double y_next = (2.0 * sigma2 / e) * (lambda - c) * y_cur
                      - sigma * sigma2 * y_prev;
        y_prev = y_cur;
        y_cur  = y_next;
        sigma  = sigma2;
    }

    return y_cur;   // C_p(λ)
}

// ---- Rayleigh-Ritz: project (A,B) into span(Yp), solve small eigenproblem ----
static void rayleigh_ritz(const Matrix& A,
                          const Matrix& B,
                          const Matrix& Yp,
                          int n_wanted,
                          Matrix& X_out,
                          std::vector<double>& Lambda_out)
{
    int m  = Yp.rows();
    int nv = Yp.cols();

    Matrix AYp(m, nv);
    mat_mult(A, Yp, AYp);
    Matrix SA(nv, nv);
    mat_mult(Yp, AYp, SA, 1.0, 0.0, 'T', 'N');

    Matrix BYp(m, nv);
    mat_mult(B, Yp, BYp);
    Matrix SB(nv, nv);
    mat_mult(Yp, BYp, SB, 1.0, 0.0, 'T', 'N');

    // Solve generalized eigenvalue problem SA * E = SB * E * Λ
    std::vector<double> w(nv);
    int info = sygv(1, 'V', 'U', nv,
                    SA.data(), SA.ld(),
                    SB.data(), SB.ld(),
                    w.data());
    if (info != 0) {
        std::cerr << "WARNING: Rayleigh-Ritz sygv returned info=" << info << "\n";
    }

    // SA now contains eigenvectors E (columns), w are eigenvalues
    // Extract the n_wanted smallest
    Lambda_out.assign(w.begin(), w.begin() + n_wanted);

    // X_out = Yp * E(:, 1:n_wanted)
    // E is stored in SA; take first n_wanted columns
    Matrix E_sub(nv, n_wanted);
    for (int j = 0; j < n_wanted; ++j)
        std::memcpy(E_sub.col(j), SA.col(j), nv * sizeof(double));

    X_out.resize(m, n_wanted);
    mat_mult(Yp, E_sub, X_out);
}

// ---- Main solver ----
SolverResult solve(const Matrix& A,
                   const Matrix& B,
                   const Matrix& Dinv,
                   const SolverConfig& cfg,
                   Matrix X0,
                   const Matrix& ref_vecs,
                   const std::vector<double>& ref_vals,
                   const Matrix& exact_Binv,
                   const Matrix& A_filter)
{
    int m = A.rows();
    int n = cfg.n_wanted;

    // Determine the A matrix used for filtering: A_filter if provided, else A.
    const Matrix& Af = (A_filter.rows() > 0) ? A_filter : A;

    // Use an expanded subspace for robustness
    int n_extra = (cfg.num_extra >= 0) ? cfg.num_extra : std::max(n / 5, 10);
    int n_sub = std::min(m, n + n_extra);

    // ---- Initial guess ----
    if (X0.rows() == 0) {
        X0.resize(m, n_sub);
        std::mt19937 rng(cfg.seed);
        std::normal_distribution<double> dist(0.0, 1.0);
        for (int j = 0; j < n_sub; ++j)
            for (int i = 0; i < m; ++i)
                X0(i, j) = dist(rng);
    } else {
        // Possibly expand to n_sub columns
        if (X0.cols() < n_sub) {
            Matrix tmp(m, n_sub);
            std::memcpy(tmp.data(), X0.data(),
                        (size_t)m * X0.cols() * sizeof(double));
            std::mt19937 rng(123);
            std::normal_distribution<double> dist(0.0, 1.0);
            for (int j = X0.cols(); j < n_sub; ++j)
                for (int i = 0; i < m; ++i)
                    tmp(i, j) = dist(rng);
            X0 = std::move(tmp);
        }
    }

    // ---- Initial Rayleigh-Ritz to get first eigenvalue estimates ----
    Matrix X = X0;
    std::vector<double> Lambda(n_sub, 0.0);
    {
        Matrix X_rr;
        std::vector<double> L_rr;
        rayleigh_ritz(A, B, X, n_sub, X_rr, L_rr);
        X = std::move(X_rr);
        Lambda = std::move(L_rr);
    }

    SolverResult result;
    result.converged = false;
    result.iters = 0;

    // ---- Record initial state (after RR, before any filter) ----
    {
        std::vector<double> wanted_lambda(Lambda.begin(), Lambda.begin() + n);
        Matrix X_wanted(m, n);
        for (int j = 0; j < n; ++j)
            std::memcpy(X_wanted.col(j), X.col(j), m * sizeof(double));

        double max_res = max_residual_norm(A, B, X_wanted, wanted_lambda);

        IterRecord rec;
        rec.iter = 0;
        rec.max_residual = max_res;
        rec.subspace_angle = -1.0;
        rec.delta_norm = -1.0;
        rec.eq5_lhs = 0.0;
        rec.eq5_rhs = 0.0;
        rec.eq5_angle = -1.0;
        rec.eq5_satisfied = false;

        if (cfg.track_angle && ref_vecs.rows() > 0 && ref_vecs.cols() >= n) {
            Matrix ref_n(m, n);
            for (int j = 0; j < n; ++j)
                std::memcpy(ref_n.col(j), ref_vecs.col(j), m * sizeof(double));
            rec.subspace_angle = max_principal_angle_sin(X_wanted, ref_n, B);
        }

        result.history.push_back(rec);

        if (cfg.verbose) {
            std::string method_name = (cfg.method == FilterMethod::ChFSI)
                                     ? "ChFSI" : "R-ChFSI";
            std::cout << "[" << method_name << "]"
                      << " iter=0 (initial RR)"
                      << "  max_residual=" << max_res;
            if (rec.subspace_angle >= 0.0)
                std::cout << "  sin_angle=" << rec.subspace_angle;
            std::cout << "\n";
        }
    }

    // Eq. (5) uses the *entering* angle — the angle of the subspace
    // BEFORE the filter is applied.  Pre-compute θ^(0) = ∠(X^(0)_{n_sub}, V_n)
    // for use at iteration 1.  Updated at the end of each iteration.
    double prev_eq5_angle = -1.0;
    if (cfg.track_eq5 && cfg.track_angle
        && ref_vecs.rows() > 0 && ref_vecs.cols() >= n) {
        Matrix ref_n(m, n);
        for (int j = 0; j < n; ++j)
            std::memcpy(ref_n.col(j), ref_vecs.col(j), m * sizeof(double));
        prev_eq5_angle = max_principal_angle_sin(X, ref_n, B);
    }

    for (int iter = 1; iter <= cfg.max_iter; ++iter) {
        // ---- Filter ----
        // ChFSI uses Af (noisy A if provided) for all operations.
        // R-ChFSI uses exact A for the initial residual, Af for recurrence.
        Matrix Yp;
        if (cfg.method == FilterMethod::ChFSI) {
            Yp = chfsi_filter(Af, Dinv, X, cfg.filter);
        } else {
            Yp = rchfsi_filter(A, B, Dinv, X, Lambda, cfg.filter, A_filter);
        }

        // ---- Compute ||delta_hat_p|| = ||B^{1/2} (Y_approx - Y_exact)|| ----
        double delta_norm_val = -1.0;
        if (cfg.track_eq5 && exact_Binv.rows() > 0) {
            Matrix Yp_exact;
            if (cfg.method == FilterMethod::ChFSI) {
                Yp_exact = chfsi_filter(A, exact_Binv, X, cfg.filter);
            } else {
                Yp_exact = rchfsi_filter(A, B, exact_Binv, X, Lambda,
                                          cfg.filter);
            }
            // Diff = Y_approx - Y_exact, all n_sub columns
            Matrix Diff(m, n_sub);
            for (int jj = 0; jj < n_sub; ++jj)
                for (int ii = 0; ii < m; ++ii)
                    Diff(ii, jj) = Yp(ii, jj) - Yp_exact(ii, jj);

            // ||delta_hat_p|| = sqrt(lambda_max(Diff^T B Diff))
            Matrix BDiff(m, n_sub);
            mat_mult(B, Diff, BDiff);
            Matrix G(n_sub, n_sub);
            mat_mult(Diff, BDiff, G, 1.0, 0.0, 'T', 'N');
            symmetrise(G);
            std::vector<double> eigs_g(n_sub);
            syev('N', 'U', n_sub, G.data(), G.ld(), eigs_g.data());
            delta_norm_val = std::sqrt(std::max(0.0, eigs_g.back()));
        }

        // ---- Rayleigh-Ritz ----
        Matrix X_new;
        std::vector<double> Lambda_new;
        rayleigh_ritz(A, B, Yp, n_sub, X_new, Lambda_new);

        X = std::move(X_new);
        Lambda = std::move(Lambda_new);

        // ---- Compute diagnostics ----
        // Residual norms (only for the n wanted eigenpairs)
        std::vector<double> wanted_lambda(Lambda.begin(), Lambda.begin() + n);
        Matrix X_wanted(m, n);
        for (int j = 0; j < n; ++j)
            std::memcpy(X_wanted.col(j), X.col(j), m * sizeof(double));

        double max_res = max_residual_norm(A, B, X_wanted, wanted_lambda);

        IterRecord rec;
        rec.iter = iter;
        rec.max_residual = max_res;
        rec.subspace_angle = -1.0;
        rec.delta_norm = delta_norm_val;
        rec.eq5_lhs = 0.0;
        rec.eq5_rhs = 0.0;
        rec.eq5_angle = -1.0;
        rec.eq5_satisfied = false;

        // ---- Subspace angle for the n wanted eigenpairs ----
        if (cfg.track_angle && ref_vecs.rows() > 0 && ref_vecs.cols() >= n) {
            Matrix ref_n(m, n);
            for (int j = 0; j < n; ++j)
                std::memcpy(ref_n.col(j), ref_vecs.col(j), m * sizeof(double));
            rec.subspace_angle = max_principal_angle_sin(X_wanted, ref_n, B);
        }

        // ---- Eq. (5) check ----
        // lhs = min_wanted |C_p(lambda_i)| - max_unwanted |C_p(lambda_i)|
        // rhs = ||delta_hat_p|| * (sec theta + csc theta)
        // theta is the *entering* angle from the previous iteration.
        if (cfg.track_eq5 && (int)ref_vals.size() > n_sub) {
            // min_{i<=n} |C_p(λ_i)| — min filter amplification among wanted eigenvalues
            double Cp_wanted_min = std::numeric_limits<double>::infinity();
            for (int i = 0; i < n; ++i)
                Cp_wanted_min = std::min(Cp_wanted_min,
                    std::abs(eval_chebyshev(ref_vals[i], cfg.filter)));
            // max_{i>n_sub} |C_p(λ_i)| — max filter leakage beyond guard vectors
            double Cp_unwanted_max = 0.0;
            for (int i = n_sub; i < (int)ref_vals.size(); ++i)
                Cp_unwanted_max = std::max(Cp_unwanted_max,
                    std::abs(eval_chebyshev(ref_vals[i], cfg.filter)));
            double cheby_gap = Cp_wanted_min - Cp_unwanted_max;

            // Compute the *current* angle (post-RR) between the full
            // n_sub-dim subspace X and the n_wanted-dim exact eigenspace.
            // This is recorded for diagnostic/plotting and becomes the
            // entering angle for the NEXT iteration.
            double angle_eq5 = -1.0;
            if (cfg.track_angle && ref_vecs.rows() > 0 && ref_vecs.cols() >= n) {
                Matrix ref_n(m, n);
                for (int j = 0; j < n; ++j)
                    std::memcpy(ref_n.col(j), ref_vecs.col(j), m * sizeof(double));
                angle_eq5 = max_principal_angle_sin(X, ref_n, B);
            }
            rec.eq5_angle = angle_eq5;

            // Use the *entering* angle (prev_eq5_angle) for the condition.
            if (delta_norm_val >= 0.0
                && prev_eq5_angle > 0.0 && prev_eq5_angle < 1.0) {
                double sin_t = prev_eq5_angle;                   // sin θ_enter
                double cos_t = std::sqrt(1.0 - sin_t * sin_t);  // cos θ_enter
                // sec θ + csc θ = (sin θ + cos θ) / (sin θ cos θ)
                double sec_plus_csc = (sin_t + cos_t) / (sin_t * cos_t);
                rec.eq5_lhs = cheby_gap;
                rec.eq5_rhs = delta_norm_val * sec_plus_csc;
                rec.eq5_satisfied = (rec.eq5_lhs > rec.eq5_rhs);
            }

            // Update entering angle for the next iteration.
            prev_eq5_angle = angle_eq5;
        }

        result.history.push_back(rec);

        if (cfg.verbose) {
            std::string method_name = (cfg.method == FilterMethod::ChFSI)
                                     ? "ChFSI" : "R-ChFSI";
            std::cout << "[" << method_name << "]"
                      << " iter=" << iter
                      << "  max_residual=" << max_res;
            if (rec.subspace_angle >= 0.0)
                std::cout << "  sin_angle=" << rec.subspace_angle;
            if (cfg.track_eq5) {
                std::cout << "  eq5_lhs=" << rec.eq5_lhs
                          << "  eq5_rhs=" << rec.eq5_rhs;
                if (rec.eq5_angle >= 0.0)
                    std::cout << "  eq5_angle=" << rec.eq5_angle;
                std::cout << (rec.eq5_satisfied ? "  [OK]" : "  [FAIL]");
            }
            std::cout << "\n";
        }

        // ---- Check convergence ----
        if (max_res < cfg.tol) {
            result.converged = true;
            result.iters = iter;
            break;
        }
        result.iters = iter;
    }

    // ---- Extract the n wanted eigenpairs ----
    result.eigvals.assign(Lambda.begin(), Lambda.begin() + n);
    result.eigvecs.resize(m, n);
    for (int j = 0; j < n; ++j)
        std::memcpy(result.eigvecs.col(j), X.col(j), m * sizeof(double));

    return result;
}

} // namespace rchfsi
