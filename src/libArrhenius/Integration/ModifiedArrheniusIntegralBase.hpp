#ifndef Integration_ModifiedArrheniusIntegralBase_hpp
#define Integration_ModifiedArrheniusIntegralBase_hpp

/** @file ModifiedArrheniusIntegralBase.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/27/17
  */

#include <boost/math/tools/roots.hpp>
using boost::math::tools::bracket_and_solve_root;
using boost::math::tools::eps_tolerance;
#include "ArrheniusIntegralBase.hpp"

namespace libArrhenius {

/** @class ModifiedArrheniusIntegralBase
  * @brief Base class for common modified Arrhenius integral data.
  * @author C.D. Clark III
  */
template<typename Real>
class ModifiedArrheniusIntegralBase : public ArrheniusIntegralBase<Real>
{
  protected:
    Real n;

  public:

    template<typename T>
    void setExponent( T n_ ) { n = n_; }
    Real getExponent( ) { return n; }

    Real getCriticalTemperature() const {
      Real Tcrit = ArrheniusIntegralBase<Real>::getCriticalTemperature();
      if( n == 0 )
        return Tcrit;

      // if n != 0, then we have a transcendental equation we need to solve.
      //
      // ln T^n - Ea/R/T + ln A = 0  (solve for T)
      //
      // use the n = 0 case as a first gues. this will be too high
      Real factor = 1.1;
      eps_tolerance<Real> tol( std::numeric_limits<Real>::digits - 3 );
      boost::uintmax_t maxit = 100;
      auto Tcrit_range =  bracket_and_solve_root(
               [&](Real T){ 
               return Constants::MKS::R*T*log(this->A*pow(T,this->n)) - this->Ea; },
               Tcrit, factor, true, tol, maxit );
      Tcrit = Tcrit_range.first;
      return Tcrit;
    }

    Real rate( Real T ) const { return pow(T,n)*this->A*exp( -this->Ea / Constants::MKS::R /T ); }

  protected:
};

}

#endif // include protector
