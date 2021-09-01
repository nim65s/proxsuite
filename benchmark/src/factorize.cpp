#include <benchmark/benchmark.h>
#include <Eigen/Cholesky>
#include <ldlt/factorize.hpp>
#include <util.hpp>

#define LDLT_BENCHMARK_MAIN BENCHMARK_MAIN    /* NOLINT */
#define LDLT_BENCHMARK BENCHMARK              /* NOLINT */
#define LDLT_BENCHMARK_TPL BENCHMARK_TEMPLATE /* NOLINT */

using namespace ldlt;

template <typename T, Layout L>
using Mat = Eigen::
		Matrix<T, -1, -1, (L == colmajor) ? Eigen::ColMajor : Eigen::RowMajor>;
template <typename T>
using Vec = Eigen::Matrix<T, -1, 1>;

template <typename T, Layout InL, Layout OutL>
void bench_eigen(benchmark::State& s) {
	i32 dim = i32(s.range(0));

	Mat<T, InL> a = ldlt_test::rand::positive_definite_rand<T>(dim, T(1e2));
	;
	Eigen::LDLT<Mat<T, OutL>> l(dim);

	benchmark::DoNotOptimize(a.data());
	benchmark::DoNotOptimize(
			const_cast /* NOLINT(cppcoreguidelines-pro-type-const-cast) */<T*>(
					l.matrixLDLT().data()));

	for (auto _ : s) {
		l.compute(a);
		benchmark::ClobberMemory();
	}
}

template <typename Strategy, typename T, Layout InL, Layout OutL>
void bench_ours(benchmark::State& s) {

	i32 dim = i32(s.range(0));
	Mat<T, InL> a = ldlt_test::rand::positive_definite_rand<T>(dim, T(1e2));
	;

	Mat<T, OutL> l(dim, dim);
	l.setZero();
	Vec<T> d(dim);
	d.setZero();

	benchmark::DoNotOptimize(a.data());
	benchmark::DoNotOptimize(l.data());
	benchmark::DoNotOptimize(d.data());

	for (auto _ : s) {
		auto a_view = detail::from_eigen_matrix(a);
		auto ldl_view = ldlt::LdltViewMut<T, OutL>{
				detail::from_eigen_matrix_mut(l),
				detail::from_eigen_vector_mut(d),
		};

		factorize(ldl_view, a_view, Strategy{});
		benchmark::ClobberMemory();
	}
}

template <typename T, Layout L>
void bench_ours_inplace(benchmark::State& s) {

	i32 dim = i32(s.range(0));
	Mat<T, InL> a = ldlt_test::rand::positive_definite_rand<T>(dim, T(1e2));

	Mat<T, L> l(dim, dim);
	l.setZero();
	Vec<T> d(dim);
	d.setZero();

	benchmark::DoNotOptimize(a.data());
	benchmark::DoNotOptimize(l.data());
	benchmark::DoNotOptimize(d.data());

	for (auto _ : s) {
		auto a_view = detail::from_eigen_matrix(a);
		auto ldl_view = ldlt::LdltViewMut<T, L>{
				detail::from_eigen_matrix_mut(l),
				detail::from_eigen_vector_mut(d),
		};

		l = a;
		factorize(ldl_view, a_view, factorization_strategy::defer_to_colmajor);
		benchmark::ClobberMemory();
	}
}

void bench_dummy(benchmark::State& s) {
	for (auto _ : s) {
	}
}

LDLT_BENCHMARK_MAIN();

constexpr i32 dim_small = 32;
constexpr i32 dim_medium = 128;
constexpr i32 dim_large = 1024;

namespace strat = factorization_strategy;

#define LDLT_BENCH(Dim, Type, InL, OutL)                                       \
	LDLT_BENCHMARK(bench_dummy);                                                 \
	LDLT_BENCHMARK_TPL(bench_eigen, Type, InL, OutL)->Arg(Dim);                  \
	LDLT_BENCHMARK_TPL(bench_ours, strat::Standard, Type, InL, OutL)->Arg(Dim);  \
	LDLT_BENCHMARK_TPL(bench_ours, strat::DeferToColMajor, Type, InL, OutL)      \
			->Arg(Dim);                                                              \
	LDLT_BENCHMARK_TPL(bench_ours, strat::DeferToRowMajor, Type, InL, OutL)      \
			->Arg(Dim)

#define LDLT_BENCH_LAYOUT(Dim, Type)                                           \
	LDLT_BENCHMARK(bench_dummy);                                                 \
	LDLT_BENCH(Dim, Type, colmajor, colmajor);                                   \
	LDLT_BENCH(Dim, Type, rowmajor, colmajor);                                   \
	LDLT_BENCH(Dim, Type, colmajor, rowmajor);                                   \
	LDLT_BENCH(Dim, Type, rowmajor, rowmajor);                                   \
	LDLT_BENCHMARK_TPL(bench_ours_inplace, Type, colmajor)->Arg(Dim);            \
	LDLT_BENCHMARK_TPL(bench_ours_inplace, Type, rowmajor)->Arg(Dim)

#define LDLT_BENCH_DIM(Type)                                                   \
	LDLT_BENCH_LAYOUT(dim_small, Type);                                          \
	LDLT_BENCH_LAYOUT(dim_medium, Type);                                         \
	LDLT_BENCH_LAYOUT(dim_large, Type)

LDLT_BENCH_DIM(f32);
LDLT_BENCH_DIM(f64);
