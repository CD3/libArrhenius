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

  public:
    ArrheniusIntegral( Real A_, Real Ea_ )
    {
      setA(A_);
      setEa(Ea_);
    }
    virtual ~ArrheniusIntegral () {};

    using ArrheniusIntegralBase<Real>::setA;
    using ArrheniusIntegralBase<Real>::setEa;



    Real operator()( std::size_t N, Real const *t, Real const *T )
    {
      Real sum = 0;
      Real alpha = -Ea/Constants::MKS::R;
      Real exp_last = exp( alpha/T[0] );
      for(size_t i = 0; i < N; i++)
      {
        Real exp_now = exp( alpha/T[i]);
        sum += (exp_now + exp_last)*(t[i]-t[i-1]);
        exp_last = exp_now;
      }
      sum *= 0.5*A;
      return sum;
    }

  protected:
};

}


#endif // include protector
