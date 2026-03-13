/// @file chebyshev_filter.cpp
/// @brief Algorithm 2 (ChFSI) and Algorithm 3 (R-ChFSI) implementations.

#include "chebyshev_filter.hpp"

#include <cassert>
#include <cmath>

namespace rchfsi {

/// Apply H = Dinv * A to a multi-vector: Y = Dinv * A * X.
static void apply_H(const Matrix& A,
                    const Matrix& Dinv,
                    const Matrix& X,
                    Matrix& Y,
                    Matrix& tmp)
{
    int m = A.rows();
    int nv = X.cols();
    mat_mult(A, X, tmp);
    mat_mult(Dinv, tmp, Y);
}

// ---- Algorithm 2: Standard ChFSI filter ----
//  Y_0 = X,  Y_1 = (sigma1/e)(H - cI)X,  three-term recurrence for Y_k.
Matrix chfsi_filter(const Matrix& A,
                    const Matrix& Dinv,
                    const Matrix& X,
                    const FilterParams& params)
{
    const int m  = X.rows();
    const int nv = X.cols();
    const int p  = params.poly_degree;

    double e = (params.lambda_max - params.lambda_cut) / 2.0;
    double c = (params.lambda_max + params.lambda_cut) / 2.0;

    double sigma  = e / (params.lambda_min - c);
    double sigma1 = sigma;
    double gamma  = 2.0 / sigma1;

    // Xprev = X (Y_0),  Ycur = (sigma1/e)(H - cI)X (Y_1)
    Matrix Xprev = X.clone();

    Matrix Ycur(m, nv);
    Matrix tmp(m, nv);
    apply_H(A, Dinv, X, Ycur, tmp);

    double s1_over_e = sigma1 / e;
    double s1c_over_e = sigma1 * c / e;
    for (int j = 0; j < nv; ++j) {
        for (int i = 0; i < m; ++i) {
            Ycur(i, j) = s1_over_e * Ycur(i, j) - s1c_over_e * Xprev(i, j);
        }
    }

    // Three-term recurrence k = 2, ..., p
    Matrix Ynext(m, nv);
    for (int k = 2; k <= p; ++k) {
        double sigma2 = 1.0 / (gamma - sigma);
        double coeff_H  = 2.0 * sigma2 / e;
        double coeff_c  = 2.0 * sigma2 * c / e;
        double coeff_prev = sigma * sigma2;

        // Ynext = coeff_H * H * Ycur  -  coeff_c * Ycur  -  coeff_prev * Xprev
        apply_H(A, Dinv, Ycur, Ynext, tmp);  // Ynext = H * Ycur
        for (int j = 0; j < nv; ++j) {
            for (int i = 0; i < m; ++i) {
                Ynext(i, j) = coeff_H * Ynext(i, j)
                            - coeff_c * Ycur(i, j)
                            - coeff_prev * Xprev(i, j);
            }
        }

        // shift:  Xprev ← Ycur,  Ycur ← Ynext
        std::swap(Xprev, Ycur);
        std::swap(Ycur, Ynext);
        sigma = sigma2;
    }

    return Ycur;   // this is Y_p
}

// ---- Algorithm 3: R-ChFSI (residual-based) filter ----
//  Recurrence in terms of residuals R = AX - BX*Lambda, robust to inexact D^{-1}.
Matrix rchfsi_filter(const Matrix& A,
                     const Matrix& B,
                     const Matrix& Dinv,
                     const Matrix& X,
                     const std::vector<double>& Lambda,
                     const FilterParams& params,
                     const Matrix& A_filter)
{
    const int m  = X.rows();
    const int nv = X.cols();
    const int p  = params.poly_degree;

    assert((int)Lambda.size() == nv);

    // Use A_filter for the recurrence operator if provided, else use A.
    const Matrix& Af = (A_filter.rows() > 0) ? A_filter : A;

    double e = (params.lambda_max - params.lambda_cut) / 2.0;
    double c = (params.lambda_max + params.lambda_cut) / 2.0;

    double sigma  = e / (params.lambda_min - c);
    double sigma1 = sigma;
    double gamma  = 2.0 / sigma1;

    // Initial residual R = A*X - B*X*diag(Lambda)
    Matrix ResY(m, nv);
    Matrix tmp(m, nv);
    mat_mult(A, X, ResY);
    mat_mult(B, X, tmp);
    for (int j = 0; j < nv; ++j)
        for (int i = 0; i < m; ++i)
            ResY(i, j) -= tmp(i, j) * Lambda[j];

    // RX = 0, RY = (sigma1/e) * ResY
    Matrix RX(m, nv);
    RX.zero();
    Matrix RY(m, nv);
    double s1_over_e = sigma1 / e;
    for (int j = 0; j < nv; ++j)
        for (int i = 0; i < m; ++i)
            RY(i, j) = s1_over_e * ResY(i, j);

    // LambdaX = I, LambdaY = (sigma1/e)*(Lambda - c)
    std::vector<double> LambdaX(nv, 1.0);
    std::vector<double> LambdaY(nv);
    for (int j = 0; j < nv; ++j)
        LambdaY[j] = s1_over_e * (Lambda[j] - c);

    // Three-term recurrence k = 2, ..., p
    Matrix Rnext(m, nv);
    for (int k = 2; k <= p; ++k) {
        double sigma2 = 1.0 / (gamma - sigma);
        double a_k = 2.0 * sigma2 / e;
        double b_k = -2.0 * sigma2 * c / e;
        double c_k = -sigma * sigma2;

        // Rnext = a_k * Af * Dinv * RY  + b_k * RY  + c_k * RX  + a_k * ResY * diag(LambdaY)
        // Step 1: tmp = Dinv * RY
        mat_mult(Dinv, RY, tmp);
        // Step 2: Rnext = Af * tmp
        mat_mult(Af, tmp, Rnext);
        // Step 3: combine
        for (int j = 0; j < nv; ++j) {
            for (int i = 0; i < m; ++i) {
                Rnext(i, j) = a_k * Rnext(i, j)
                            + b_k * RY(i, j)
                            + c_k * RX(i, j)
                            + a_k * ResY(i, j) * LambdaY[j];
            }
        }

        // Update Λ:  ΛX_next = a_k * ΛY * Λ + b_k * ΛY + c_k * ΛX
        std::vector<double> LambdaNext(nv);
        for (int j = 0; j < nv; ++j) {
            LambdaNext[j] = a_k * LambdaY[j] * Lambda[j]
                          + b_k * LambdaY[j]
                          + c_k * LambdaX[j];
        }

        // shift
        std::swap(RX, RY);
        std::swap(RY, Rnext);
        LambdaX = LambdaY;
        LambdaY = LambdaNext;
        sigma = sigma2;
    }

    // Reconstruct Y_p = Dinv * RY + X * diag(LambdaY)
    Matrix Yp(m, nv);
    mat_mult(Dinv, RY, Yp);
    for (int j = 0; j < nv; ++j)
        for (int i = 0; i < m; ++i)
            Yp(i, j) += X(i, j) * LambdaY[j];   // Yp += X * diag(ΛY)

    return Yp;
}

} // namespace rchfsi
