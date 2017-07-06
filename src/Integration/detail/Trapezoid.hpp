#ifndef Integration_Methods_Trapezoid_hpp
#define Integration_Methods_Trapezoid_hpp


/** @file Trapezoid.hpp
  * @brief Contains ArrheniusIntegral class specialization for trapezoid method.
  * @author C.D. Clark III
  * @date 06/26/17
  *
  * NOTE: This file is expected to be included from the ArrheniusIntegral.hpp
  * It does not include any of the headers that are already included there, which
  * only works if the contents of ArrheniusIntegral.hpp is included first.
  */

namespace libArrhenius {

template <typename Real>
class ArrheniusIntegral<Real,Trapezoid> : public ArrheniusIntegralBase<Real>
{
  protected:
    // this will keep up from having to use 'this->' to access these.
    using ArrheniusIntegralBase<Real>::Ea;
    using ArrheniusIntegralBase<Real>::A;
    using ArrheniusIntegralBase<Real>::parallel_threshold;

  public:
    ArrheniusIntegral( Real A_, Real Ea_ )
    {
      this->setA(A_);
      this->setEa(Ea_);
    }
    ArrheniusIntegral( )
    {}
    virtual ~ArrheniusIntegral () {};


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
        Real exp_now = exp( alpha/T[i] ); \
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
    }

  protected:
};

}


#endif // include protector
