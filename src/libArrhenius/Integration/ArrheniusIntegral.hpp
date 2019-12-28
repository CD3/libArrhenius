#ifndef Integration_ArrheniusIntegral_hpp
#define Integration_ArrheniusIntegral_hpp

/** @file ArrheniusIntegral.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/26/17
  */

#include <cmath>

#include "../Constants.hpp"
#include "./ArrheniusIntegralBase.hpp"
#include "./detail/Utils.hpp"

namespace libArrhenius {


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
#include "./detail/ArrheniusIntegral/Trapezoid.hpp"


#endif // include protector
