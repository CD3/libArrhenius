#ifndef Integration_ArrheniusIntegral_hpp
#define Integration_ArrheniusIntegral_hpp

/** @file ArrheniusIntegral.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/26/17
  */

#include <cmath>

#include "Constants.hpp"
#include "ArrheniusIntegralBase.hpp"

namespace libArrhenius {


namespace ArrheniusIntegralDetail
{
// we need an add function that can be used to reduce user-defined types with
// openmp
template<typename Real>
Real add(Real r, Real n)
{
  return r+n;
}
}


struct Trapezoid {};

/** @class ArrheniusIntegral
  * @brief 
  * @author C.D. Clark III
  */
template <typename Real, typename Method = Trapezoid >
class ArrheniusIntegral {};

}

// include specific implementations here as they
// won't work if the user tries to include them directly
#include "./detail/Trapezoid.hpp"


#endif // include protector
