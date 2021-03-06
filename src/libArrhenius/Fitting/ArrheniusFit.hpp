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
struct MinimizeScalingFactors {};
struct EffectiveExposuresLinearRegression {};
struct ConstantTemperatureLinearRegression {};

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
#include "./detail/MinimizeScalingFactors.hpp"
#include "./detail/EffectiveExposuresLinearRegression.hpp"
#include "./detail/ConstantTemperatureLinearRegression.hpp"

#endif // include protector
