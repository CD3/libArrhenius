#ifndef Integrators_ArrheniusIntegral_Trapezoid_hpp
#define Integrators_ArrheniusIntegral_Trapezoid_hpp

/** @file ArrheniusIntegral_Trapezoid.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/26/17
  */

#include "ArrheniusIntegralBase.hpp"

namespace libArrhenius {

template <typename Real>
class ArrheniusIntegral<Real,Trapezoid> : ArrheniusIntegralBase<Real>
{
  protected:
    using ArrheniusIntegralBase<Real>::A;
    using ArrheniusIntegralBase<Real>::Ea;
    using ArrheniusIntegralBase<Real>::parallel_threshold;

  public:
    ArrheniusIntegral( Real A_, Real Ea_ )
    {
      setA(A_);
      setEa(Ea_);
    }
    ArrheniusIntegral( )
    {}
    virtual ~ArrheniusIntegral () {};

    using ArrheniusIntegralBase<Real>::setA;
    using ArrheniusIntegralBase<Real>::setEa;



    Real operator()( std::size_t N, Real const *t, Real const *T ) const
    {
      Real sum = 0;
      Real alpha = -Ea/Constants::MKS::R;
      // we need to have some way for each thread to tell if they have cached the first
      // calculation or not.
      Real exp_last;
      bool have_last = false;

      // see the celero benchmarks.
      // using tmp variables and caching calls to exp() is *about*
      // 3 times faster.
      // parallelization can cost more than it saves on small for loops.
#define LOOP \
      for(size_t i = 1; i < N; ++i) \
      { \
        double exp_now = exp( alpha/T[i] ); \
        if(!have_last) \
        { \
          exp_last = exp_now; \
          have_last = true; \
          continue; \
        } \
        sum += (exp_now + exp_last)*(t[i]-t[i-1]); \
        exp_last = exp_now; \
      } \

      if(N < parallel_threshold)
      {
        LOOP
      }
      else
      {
        #pragma omp parallel for schedule(static) reduction(+:sum) firstprivate(have_last) private(exp_last)
        LOOP
      }
      sum *= 0.5*A;
#undef LOOP
      return sum;









      //Real sum = 0;
      //Real alpha = -Ea/Constants::MKS::R;
      //Real exp_last = exp( alpha/T[0] );
      //for(size_t i = 1; i < N; i++)
      //{
        //Real exp_now = exp( alpha/T[i]);
        //sum += (exp_now + exp_last)*(t[i]-t[i-1]);
        //exp_last = exp_now;
      //}
      //sum *= 0.5*A;
      //return sum;
    }

  protected:
};

}


#endif // include protector
