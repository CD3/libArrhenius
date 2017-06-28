#include <celero/Celero.h>

#include <boost/optional.hpp>

#ifndef WIN32
#include <cmath>
#include <cstdlib>
#endif

CELERO_MAIN

// fixture to setup benchmarks for different array sizes
class Fixture : public celero::TestFixture
{
  public:

    Fixture()
    {
    }

    virtual std::vector<std::pair<int64_t, uint64_t>> getExperimentValues() const override
    {
      std::vector<std::pair<int64_t, uint64_t>> problemSpace;
      const int runs = 6;
      for( int i = 0; i < runs; i++)
      {
        problemSpace.push_back(std::make_pair(int64_t(pow(2,2*i+6)),uint64_t(0)));
      }

      return problemSpace;
    }
    virtual void setUp(int64_t N)
    {
      this->size = N;
      this->t = std::vector<double>(N);
      this->T = std::vector<double>(N);
      for(int64_t i = 0; i < this->size; i++)
      {
        this->t[i] = i;
        this->T[i] = i;
      }
      A = 3.1e99;
      Ea = 6.28e5;

    }


    int64_t size;
    std::vector<double> t;
    std::vector<double> T;
    double A, Ea;


};


// Straight forward trapezoid rule with no optimization
BASELINE_F(TrapezoidIntegral, Baseline, Fixture, 100, 100)
{
  double sum = 0;
  for(int64_t i = 0; i < size-1; i++)
    sum += 0.5*A*( exp( -Ea/8.314/T[i]) + exp( -Ea/8.314/T[i+1]) )*(t[i+1]-t[i]);
}

// is ++i really faster?
// answer is no...
//BENCHMARK_F(TrapezoidIntegral, PreIncrement, Fixture, 100, 100)
//{
  //double sum = 0;
  //for(int64_t i = 0; i < size-1; ++i)
    //sum += 0.5*A*( exp( -Ea/8.314/T[i]) + exp( -Ea/8.314/T[i+1]) )*(t[i+1]-t[i]);
//}

// how much difference does pulling out the constants make?
// not much...
//BENCHMARK_F(TrapezoidIntegral, FactorConstants, Fixture, 100, 100)
//{
  //double sum = 0;
  //for(int64_t i = 0; i < size-1; ++i)
    //sum += ( exp( -Ea/8.314/T[i]) + exp( -Ea/8.314/T[i+1]) )*(t[i+1]-t[i]);
  //sum *= 0.5*A;
//}

// we can be smart and use some tmp variables
// now we're talking
// knocks off about 30%
//BENCHMARK_F(TrapezoidIntegral, TmpVars, Fixture, 100, 100)
//{
    //double sum = 0;
    //double alpha = -Ea/8.314;
    //for(int64_t i = 0; i < size-1; i++)
      //sum += ( exp( alpha/T[i]) + exp( alpha/T[i+1]) )*(t[i+1]-t[i]);
    //sum *= 0.5*A;
//}

// what if we use tmp vars and try to cache the exp calculations?
// knocks off about 60%
BENCHMARK_F(TrapezoidIntegral, TmpVarsAndCache, Fixture, 100, 100)
{
    double sum = 0;
    double alpha = -Ea/8.314;
    double exp_last = exp( alpha/T[0] );
    for(int64_t i = 1; i < this->size; i++)
    {
      double exp_now = exp( alpha/T[i]);
      sum += (exp_now + exp_last)*(t[i]-t[i-1]);
      exp_last = exp_now;
    }
    sum *= 0.5*A;
}

// what about just straight parallelization?
BENCHMARK_F(TrapezoidIntegral, OpenMPParallel, Fixture, 100, 100)
{
    double sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for(int64_t i = 0; i < this->size-1; i++)
      sum += ( exp( -Ea/8.314/T[i]) + exp( -Ea/8.314/T[i+1]) )*(t[i+1]-t[i]);
    sum *= 0.5*A;
}

BENCHMARK_F(TrapezoidIntegral, OpenMPParallelAndCache, Fixture, 100, 100)
{
    double sum = 0;
    double alpha = -Ea/8.314;
    // we need to have some way for each thread to tell if they have cached the first
    // calculation or not.
    double exp_last;
    bool have_last = False;
    #pragma omp parallel for reduction(+:sum) firstprivate(have_last,exp_last) schedule(static,256)
    for(int64_t i = 1; i < this->size; i++)
    {
      double exp_now = exp( alpha/T[i] );
      if(!have_last)
      {
        exp_last = exp_now;
        have_last = True;
        continue;
      }
      sum += (exp_now + exp_last.get())*(t[i]-t[i-1]);
      exp_last = exp_now;
    }
    sum *= 0.5*A;
}

