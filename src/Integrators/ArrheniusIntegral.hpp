#ifndef Integrators_ArrheniusIntegral_hpp
#define Integrators_ArrheniusIntegral_hpp

/** @file ArrheniusIntegral.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/26/17
  */

#include <cmath>

#include "Types.hpp"
#include "Constants.hpp"

namespace libArrhenius {

/** @class ArrheniusIntegral
  * @brief 
  * @author C.D. Clark III
  */
template <typename Real, typename Method = Trapezoid >
class ArrheniusIntegral {};

}

// include specific implementations here as they
// cannot be included by the user directly.
#include "ArrheniusIntegral_Trapezoid.hpp"


#endif // include protector
