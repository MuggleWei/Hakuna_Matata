#include "benchmark/benchmark.h"
#include <atomic>
#include <math.h>

static std::atomic<int> cnt;

int is_prime(unsigned long long n)
{
	unsigned long long i;
	if (n == 2) {
		return 1;
	}

	if (n % 2 == 0) {
		return 0;
	}
	for (i = 3; i < sqrt(n); i = i + 2) {
		if (n % i == 0) {
			return 0;
		}
	}
	return 1;
}

class HelloFixture : public benchmark::Fixture {
public:
	virtual void SetUp(const benchmark::State &) override
	{
		cnt = 0;
	}
	virtual void TearDown(const benchmark::State &) override
	{
	}
};

BENCHMARK_DEFINE_F(HelloFixture, run)(benchmark::State &state)
{
	unsigned long long n = 1;
	unsigned int n_prime = 0;
	for (auto _ : state) {
		if (is_prime(n++)) {
			++n_prime;
		}
	}
	cnt.fetch_add(n, std::memory_order_relaxed);
}

#define RUN_BENCH(fixture, name, n_thread) \
	BENCHMARK_REGISTER_F(fixture, name)    \
		->Threads(n_thread)                \
		->Iterations(10000)                \
		->Repetitions(5)

RUN_BENCH(HelloFixture, run, 1);
RUN_BENCH(HelloFixture, run, 2);
RUN_BENCH(HelloFixture, run, 4);
RUN_BENCH(HelloFixture, run, 8);

BENCHMARK_MAIN();
