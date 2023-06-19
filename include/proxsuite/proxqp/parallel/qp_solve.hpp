//
// Copyright (c) 2023 INRIA
//
/** \file */
#ifndef PROXSUITE_PROXQP_PARALLEL_QPSOLVE_HPP
#define PROXSUITE_PROXQP_PARALLEL_QPSOLVE_HPP

#include "proxsuite/proxqp/dense/wrapper.hpp"
#include "proxsuite/proxqp/sparse/wrapper.hpp"
#include "proxsuite/proxqp/parallel/omp.hpp"

namespace proxsuite {
namespace proxqp {

template<typename T>
void
solve_in_parallel(std::vector<proxqp::dense::QP<T>>& qps,
                  optional<const size_t> num_threads = nullopt)
{
  if (num_threads != nullopt) {
    set_default_omp_options(num_threads.value());
  } else {
    size_t NUM_THREADS =
      std::max((size_t)(omp_get_max_threads() / 2), (size_t)(1));
    set_default_omp_options(NUM_THREADS);
  }
  typedef proxqp::dense::QP<T> qp_dense;
  const Eigen::DenseIndex batch_size = qps.size();
  Eigen::DenseIndex i = 0;
#pragma omp parallel for schedule(dynamic)
  for (i = 0; i < batch_size; i++) {
    qp_dense& qp = qps[i];
    qp.solve();
  }
}

template<typename T>
void
solve_in_parallel(proxqp::dense::BatchQP<T>& qps,
                  optional<const size_t> num_threads = nullopt)
{
  if (num_threads != nullopt) {
    set_default_omp_options(num_threads.value());
  } else {
    size_t NUM_THREADS =
      std::max((size_t)(omp_get_max_threads() / 2), (size_t)(1));
    set_default_omp_options(NUM_THREADS);
  }
  typedef proxqp::dense::QP<T> qp_dense;
  const Eigen::DenseIndex batch_size = qps.size();
  Eigen::DenseIndex i = 0;
#pragma omp parallel for schedule(dynamic)
  for (i = 0; i < batch_size; i++) {
    qp_dense& qp = qps[i];
    qp.solve();
  }
}

template<typename T, typename I>
void
solve_in_parallel(proxqp::sparse::BatchQP<T, I>& qps,
                  optional<const size_t> num_threads = nullopt)
{
  if (num_threads != nullopt) {
    set_default_omp_options(num_threads.value());
  } else {
    size_t NUM_THREADS =
      std::max((size_t)(omp_get_max_threads() / 2), (size_t)(1));
    set_default_omp_options(NUM_THREADS);
  }

  typedef proxqp::sparse::QP<T, I> qp_sparse;
  const Eigen::DenseIndex batch_size = qps.size();
  Eigen::DenseIndex i = 0;
#pragma omp parallel for schedule(dynamic)
  for (i = 0; i < batch_size; i++) {
    qp_sparse& qp = qps[i];
    qp.solve();
  }
}

template<typename T, typename I>
void
solve_in_parallel(std::vector<proxqp::sparse::QP<T, I>>& qps,
                  optional<const size_t> num_threads = nullopt)
{
  if (num_threads != nullopt) {
    set_default_omp_options(num_threads.value());
  } else {
    size_t NUM_THREADS =
      std::max((size_t)(omp_get_max_threads() / 2), (size_t)(1));
    set_default_omp_options(NUM_THREADS);
  }

  typedef proxqp::sparse::QP<T, I> qp_sparse;
  const Eigen::DenseIndex batch_size = qps.size();
  Eigen::DenseIndex i = 0;
#pragma omp parallel for schedule(dynamic)
  for (i = 0; i < batch_size; i++) {
    qp_sparse& qp = qps[i];
    qp.solve();
  }
}

} // namespace proxqp
} // namespace proxsuite

#endif /* end of include guard PROXSUITE_PROXQP_PARALLEL_QPSOLVE_HPP */
