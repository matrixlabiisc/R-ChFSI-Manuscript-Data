#pragma once
/// @file linalg.hpp
/// @brief BLAS / LAPACK free-function wrappers.

#include <vector>

namespace rchfsi {

// ---- BLAS-3 ----
void gemm(char transA, char transB,
          int m, int n, int k,
          double alpha,
          const double* A, int ldA,
          const double* B, int ldB,
          double beta,
          double* C, int ldC);

// ---- BLAS-1 ----
double nrm2(int n, const double* x, int incx);

// ---- LAPACK ----
int syev(char jobz, char uplo, int n,
         double* A, int ldA, double* w);

int sygv(int itype, char jobz, char uplo, int n,
         double* A, int ldA, double* B, int ldB,
         double* w);

/// Largest singular value of a general matrix (buffer may be overwritten).
double spectral_norm(int m, int n, double* A, int ldA);

} // namespace rchfsi
