/// @file linalg.cpp
/// @brief Free-function wrappers around Fortran BLAS / LAPACK.

#include "linalg.hpp"

#include <algorithm>
#include <vector>

extern "C" {
    void dgemm_(const char*, const char*,
                const int*, const int*, const int*,
                const double*, const double*, const int*,
                const double*, const int*,
                const double*, double*, const int*);
    double dnrm2_(const int*, const double*, const int*);
    void dsyev_(const char*, const char*,
                const int*, double*, const int*,
                double*, double*, const int*, int*);
    void dsygv_(const int*, const char*, const char*,
                const int*, double*, const int*,
                double*, const int*, double*,
                double*, const int*, int*);
    void dgesvd_(const char*, const char*,
                 const int*, const int*,
                 double*, const int*,
                 double*,
                 double*, const int*,
                 double*, const int*,
                 double*, const int*, int*);
}

namespace rchfsi {

void gemm(char transA, char transB,
          int m, int n, int k,
          double alpha,
          const double* A, int ldA,
          const double* B, int ldB,
          double beta,
          double* C, int ldC)
{
    dgemm_(&transA, &transB, &m, &n, &k,
           &alpha, A, &ldA, B, &ldB,
           &beta, C, &ldC);
}

double nrm2(int n, const double* x, int incx)
{
    return dnrm2_(&n, x, &incx);
}

int syev(char jobz, char uplo, int n,
         double* A, int ldA, double* w)
{
    int lwork = -1, info = 0;
    double work_query = 0.0;
    dsyev_(&jobz, &uplo, &n, A, &ldA, w, &work_query, &lwork, &info);
    lwork = static_cast<int>(work_query);
    std::vector<double> work(lwork);
    dsyev_(&jobz, &uplo, &n, A, &ldA, w, work.data(), &lwork, &info);
    return info;
}

int sygv(int itype, char jobz, char uplo, int n,
         double* A, int ldA, double* B, int ldB,
         double* w)
{
    int lwork = -1, info = 0;
    double work_query = 0.0;
    dsygv_(&itype, &jobz, &uplo, &n, A, &ldA, B, &ldB,
           w, &work_query, &lwork, &info);
    lwork = static_cast<int>(work_query);
    std::vector<double> work(lwork);
    dsygv_(&itype, &jobz, &uplo, &n, A, &ldA, B, &ldB,
           w, work.data(), &lwork, &info);
    return info;
}

double spectral_norm(int m, int n, double* A, int ldA)
{
    int mn = std::min(m, n);
    std::vector<double> S(mn);
    std::vector<double> work_tmp(1);
    int lwork = -1, info = 0;
    char jobu = 'N', jobvt = 'N';
    dgesvd_(&jobu, &jobvt, &m, &n, A, &ldA,
            S.data(), nullptr, &m, nullptr, &n,
            work_tmp.data(), &lwork, &info);
    lwork = static_cast<int>(work_tmp[0]);
    std::vector<double> work(lwork);
    dgesvd_(&jobu, &jobvt, &m, &n, A, &ldA,
            S.data(), nullptr, &m, nullptr, &n,
            work.data(), &lwork, &info);
    return S.empty() ? 0.0 : S[0];
}

} // namespace rchfsi
