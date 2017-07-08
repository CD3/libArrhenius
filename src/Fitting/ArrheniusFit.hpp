#ifndef Fitting_ArrheniusFit_hpp
#define Fitting_ArrheniusFit_hpp

/** @file ArrheniusFit.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/29/17
  */

#include "ArrheniusFitBase.hpp"

namespace libArrhenius {

struct MinimizeLogAVarianceAndScalingFactors {};
struct EffectiveExposuresLinearRegression {};
struct ConstantTempeLinearRegression {};

/** @class ArrheniusFit
  * @brief 
  * @author C.D. Clark III
  */
template<typename Real, typename Method = MinimizeLogAVarianceAndScalingFactors>
class ArrheniusFit {};

}

// include specific implementations here as they
// won't work if the user tries to include them directly
#include "./detail/MinimizeLogAVarianceAndScalingFactors.hpp"
#include "./detail/EffectiveExposuresLinearRegression.hpp"

#endif // include protector
