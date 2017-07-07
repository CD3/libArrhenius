#include "benchmark/benchmark.h"
#include <string>
#include <cstring>

// Google's benchmark framework
//
// To define a benchmark you simply write a function that takes a benchmark::State reference
// as argument (the state instance allows the framework to pass information into your benchmark).
// You put the code you want to time inside a while loop that tests the benchmark::State::KeepRunning() method
// of the state instance.
//
// Then, you register your function as a benchmark using the BENCHMARK macro.
//
// It's even possible to pass variables into you benchmark (see https://github.com/google/benchmark for more information)

static void BM_StringCreation(benchmark::State& state) {
  while (state.KeepRunning())
    std::string empty_string;
}
// Register the function as a benchmark
BENCHMARK(BM_StringCreation);

// Define another benchmark
static void BM_StringCopy(benchmark::State& state) {
  std::string x = "hello";
  while (state.KeepRunning())
    std::string copy(x);
}
BENCHMARK(BM_StringCopy);

static void BM_memcpy(benchmark::State& state) {
  char* src = new char[state.range(0)];
  char* dst = new char[state.range(0)];
  std::memset(src, 'x', state.range(0));
  while (state.KeepRunning())
    memcpy(dst, src, state.range(0));
  state.SetBytesProcessed(int64_t(state.iterations()) *
                          int64_t(state.range(0)));
  delete[] src;
  delete[] dst;
}
BENCHMARK(BM_memcpy)->Arg(8)->Arg(64)->Arg(512)->Arg(1<<10)->Arg(8<<10);
