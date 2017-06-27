#define NONIUS_RUNNER
#include "nonius/nonius.h++"
#include "nonius/main.h++"

#include <vector>


NONIUS_BENCHMARK("100 element vector set",
[](nonius::chronometer meter)
{
  // anything out here won't be measured
  std::vector<double> v(100);

  meter.measure( [&](){
      // this is measured
      for(int i = 0; i < 100; ++i)
        v[i] = i;
      } );

  // anything out here won't be measured either
})

NONIUS_BENCHMARK("100 element raw pointer set",
[](nonius::chronometer meter)
{
  // anything out here won't be measured
  double *v = new double[100];

  meter.measure( [&](){
      // this is measured
      for(int i = 0; i < 100; ++i)
        v[i] = i;
      } );

  // anything out here won't be measured either
  delete[] v;
})




NONIUS_BENCHMARK("1000 element vector set",
[](nonius::chronometer meter)
{
  // anything out here won't be measured
  std::vector<double> v(1000);

  meter.measure( [&](){
      // this is measured
      for(int i = 0; i < 1000; ++i)
        v[i] = i;
      } );

  // anything out here won't be measured either
})

NONIUS_BENCHMARK("1000 element raw pointer set",
[](nonius::chronometer meter)
{
  // anything out here won't be measured
  double *v = new double[1000];

  meter.measure( [&](){
      // this is measured
      for(int i = 0; i < 1000; ++i)
        v[i] = i;
      } );

  // anything out here won't be measured either
  delete[] v;
})

