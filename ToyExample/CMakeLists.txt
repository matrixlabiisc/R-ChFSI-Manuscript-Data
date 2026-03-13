cmake_minimum_required(VERSION 3.16)
project(rchfsi_toy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(BLAS   REQUIRED)
find_package(LAPACK REQUIRED)

# ----- Library target -----
add_library(rchfsi
    src/linalg.cpp
    src/chebyshev_filter.cpp
    src/eigensolver.cpp
    src/test_problems.cpp
)

target_include_directories(rchfsi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(rchfsi PUBLIC
    ${LAPACK_LIBRARIES}
    ${BLAS_LIBRARIES}
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    find_library(GFORTRAN_LIB gfortran)
    if(GFORTRAN_LIB)
        target_link_libraries(rchfsi PUBLIC ${GFORTRAN_LIB})
    endif()
endif()

# ----- Paper data generator -----
add_executable(plot_convergence examples/plot_convergence.cpp)
target_link_libraries(plot_convergence PRIVATE rchfsi)
