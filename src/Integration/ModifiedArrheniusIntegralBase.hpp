#ifndef Integration_ModifiedArrheniusIntegralBase_hpp
#define Integration_ModifiedArrheniusIntegralBase_hpp

/** @file ModifiedArrheniusIntegralBase.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/27/17
  */

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

  protected:
};

}

#endif // include protector
