#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"

#include <libArrhenius/Integration/ArrheniusIntegral.hpp>

using namespace libArrhenius;
using namespace libArrhenius::Constants;


TEST_CASE( "Integration Performance", "[.][benchmarks]" ) {

  double              tau = 2;
  double              dt  = tau / 20;
  size_t              N   = 4 * tau / dt;
  std::vector<double> t(N), T(N);

  for (size_t i = 0; i < t.size(); i++) {
    t[i] = dt * i;
    T[i] = 310;
    if (t[i] > tau / 2) T[i] = 100 + 310;
    if (t[i] > tau + tau / 2) T[i] = 310;
  }

  SECTION("Default")
  {
    ArrheniusIntegral<double> Arr(3.1e99, 6.28e5);

    BENCHMARK("Integrate")
    {
      return Arr(N,t.data(),T.data());
    };
  }


}
