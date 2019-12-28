#ifndef Integration_ModifiedArrheniusIntegral_hpp
#define Integration_ModifiedArrheniusIntegral_hpp

/** @file ModifiedArrheniusIntegral.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/26/17
  */

#include <cmath>

#include "../Constants.hpp"
#include "./ModifiedArrheniusIntegralBase.hpp"
#include "./ArrheniusIntegral.hpp"

namespace libArrhenius {



/** @class ModifiedArrheniusIntegral
  * @brief Integrates the "modifed" Arrhenius equation.
  * @author C.D. Clark III
  */
template <typename Real, typename Method = Trapezoid >
class ModifiedArrheniusIntegral {};

}

// include specific implementations here as they
// won't work if the user tries to include them directly
#include "./detail/ModifiedArrheniusIntegral/Trapezoid.hpp"


#endif // include protector
