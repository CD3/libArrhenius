#ifndef Integration_detail_ExponentialIntegral_hpp
#define Integration_detail_ExponentialIntegral_hpp

#include <boost/math/special_functions/expint.hpp>
#include <iostream>


/** @file ExponentialIntegral.hpp
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
class ArrheniusIntegral<Real,ExponentialIntegral> : public ArrheniusIntegralBase<Real>
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


    Real operator()( std::size_t N, Real const *t, Real const *T ) const
    {
      Real sum = 0;
      Real alpha = Ea/Constants::MKS::R;
      Real tolerance = 0.001/alpha;
      // we need to have some way for each thread to tell if they have cached the first
      // calculation or not.
      Real quadrature_last;
      bool have_last = false;

#define LOOP \
      for(size_t i = 1; i < N; ++i) \
      { \
        Real quadrature_now = T[i]*boost::math::expint(2,alpha/T[i] ); \
        if(!have_last) \
        { \
          quadrature_last = quadrature_now; \
          have_last = true; \
          continue; \
        } \
        if( abs(1/T[i] - 1/T[i-1]) > tolerance ) \
        { \
          sum += (quadrature_now - quadrature_last)*(t[i]-t[i-1])/(T[i] - T[i-1]); \
          quadrature_last = quadrature_now; \
        } \
        else \
        { \
          sum += exp(-alpha/T[i])*(t[i] - t[i-1]); \
        } \
      } \

      if(N < parallel_threshold)
      {
        LOOP
      }
      else
      {
        // we have to use a special reduction function here
        #pragma omp declare reduction( add:Real:omp_out=Integration::detail::add(omp_out, omp_in) )initializer(omp_priv=0)
        #pragma omp parallel for schedule(static) reduction(add:sum) firstprivate(have_last) private(quadrature_last)
        LOOP
      }
      sum *= A;
#undef LOOP
      return sum;
    }

};

}


#endif // include protector
