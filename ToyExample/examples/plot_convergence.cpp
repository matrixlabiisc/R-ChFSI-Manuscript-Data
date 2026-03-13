/// @file plot_convergence.cpp
/// @brief Paper data generator: runs ChFSI/R-ChFSI on standard (B=I) and
///        generalized (B!=I) problems across noise levels, writes .dat files.

#include "eigensolver.hpp"
#include "test_problems.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace rchfsi;

static void write_dat(const std::string& fname,
                      const std::vector<IterRecord>& hist)
{
    std::ofstream f(fname);
    f << "# iter  max_residual  sin_angle  delta_norm  eq5_lhs  eq5_rhs  eq5_angle  eq5_ok\n";
    for (auto& r : hist)
        f << r.iter << "  "
          << std::scientific << std::setprecision(8) << r.max_residual << "  "
          << r.subspace_angle << "  "
          << r.delta_norm << "  "
          << r.eq5_lhs << "  "
          << r.eq5_rhs << "  "
          << r.eq5_angle << "  "
          << (r.eq5_satisfied ? 1 : 0) << "\n";
    f.close();
    std::cout << "  wrote " << fname << "  (" << hist.size() << " rows)\n";
}

// ---- Standard eigenproblem (B = I, varying noise) ----
static void dump_standard(const std::string& dir)
{
    std::cout << "\n--- Standard eigenproblem (B=I) ---\n";

    const int m = 1000;
    const int n_wanted = 10;
    const int poly_deg = 8;
    const int max_it   = 100;
    const int num_extra = 0;
    const int n_sub = n_wanted + num_extra;

    // Eigenvalue spectrum:
    //   wanted:   1.0, ..., 4.0  (100 values, spacing ~0.03)
    //   unwanted: 5.0, 5.2, ...  (gap = 1.0)
    std::vector<double> eigs(m);
    for (int i = 0; i < n_wanted; ++i)
        eigs[i] = 1.0 + 3.0 * i / (n_wanted - 1);        // wanted: [1.0, 4.0]
    for (int i = n_wanted; i < m; ++i)
        eigs[i] = 5.0 + 0.2 * (i - n_wanted);             // unwanted: [5.0, ...]

    std::vector<double> noise_levels = {0.0, 1e-4, 1e-3, 1e-2};

    for (size_t ni = 0; ni < noise_levels.size(); ++ni) {
        double noise = noise_levels[ni];
        auto prob = build_standard_problem(eigs, noise, 42);

        for (auto method : {FilterMethod::ChFSI, FilterMethod::RChFSI}) {
            SolverConfig cfg;
            cfg.method         = method;
            cfg.n_wanted       = n_wanted;
            cfg.num_extra      = num_extra;
            cfg.max_iter       = max_it;
            cfg.tol            = 1e-15;   // never stop early → full curve
            cfg.verbose        = true;
            cfg.track_angle    = true;
            cfg.track_eq5      = true;

            cfg.filter.poly_degree = poly_deg;
            cfg.filter.lambda_min  = eigs.front() - 0.05;
            cfg.filter.lambda_cut  = (eigs[n_sub - 1] + eigs[n_sub]) / 2.0;
            cfg.filter.lambda_max  = eigs.back() + 0.1;

            auto result = solve(prob.A, prob.B, prob.Dinv, cfg,
                                {}, prob.exact_eigvecs, prob.exact_eigvals,
                                prob.Binv, prob.A_filter);

            std::string mname = (method == FilterMethod::ChFSI) ? "chfsi" : "rchfsi";
            std::string fname = dir + "/conv_standard_" + mname
                              + "_noise" + std::to_string(ni) + ".dat";
            write_dat(fname, result.history);
        }
    }
}

// ---- Generalized eigenproblem (B != I, varying zeta) ----
static void dump_generalized(const std::string& dir)
{
    std::cout << "\n--- Generalized eigenproblem (B!=I) ---\n";

    const int m = 1000;
    const int n_wanted = 10;
    const int poly_deg = 8;
    const int max_it   = 100;
    const int num_extra = 0;
    const int n_sub = n_wanted + num_extra;

    // Same spectrum for A-eigenvalues
    std::vector<double> a_eigs(m);
    for (int i = 0; i < n_wanted; ++i)
        a_eigs[i] = 1.0 + 3.0 * i / (n_wanted - 1);
    for (int i = n_wanted; i < m; ++i)
        a_eigs[i] = 5.0 + 0.2 * (i - n_wanted);

    // B eigenvalues spread out → non-trivial B
    std::vector<double> b_eigs(m);
    for (int i = 0; i < m; ++i)
        b_eigs[i] = 1.0 + 4.0 * i / (m - 1);

    std::vector<double> zeta_values = {0.0, 1e-4, 1e-3, 1e-2};

    for (size_t zi = 0; zi < zeta_values.size(); ++zi) {
        double zeta = zeta_values[zi];
        auto prob = build_test_problem(a_eigs, b_eigs, zeta, true, 42);

        for (auto method : {FilterMethod::ChFSI, FilterMethod::RChFSI}) {
            SolverConfig cfg;
            cfg.method         = method;
            cfg.n_wanted       = n_wanted;
            cfg.num_extra      = num_extra;
            cfg.max_iter       = max_it;
            cfg.tol            = 1e-15;   // never stop early
            cfg.verbose        = true;
            cfg.track_angle    = true;
            cfg.track_eq5      = true;

            cfg.filter.poly_degree = poly_deg;
            cfg.filter.lambda_min  = prob.exact_eigvals.front() - 0.05;
            cfg.filter.lambda_cut  = (prob.exact_eigvals[n_sub - 1]
                                    + prob.exact_eigvals[n_sub]) / 2.0;
            cfg.filter.lambda_max  = prob.exact_eigvals.back() + 0.1;

            auto result = solve(prob.A, prob.B, prob.Dinv, cfg,
                                {}, prob.exact_eigvecs, prob.exact_eigvals,
                                prob.Binv);

            std::string mname = (method == FilterMethod::ChFSI) ? "chfsi" : "rchfsi";
            std::string fname = dir + "/conv_generalized_" + mname
                              + "_zeta" + std::to_string(zi) + ".dat";
            write_dat(fname, result.history);
        }
    }
}

// ----
int main(int argc, char** argv)
{
    std::string outdir = ".";
    if (argc > 1) outdir = argv[1];

    dump_standard(outdir);
    dump_generalized(outdir);
    std::cout << "\nDone. Data files written to " << outdir << "/\n";
    return 0;
}
