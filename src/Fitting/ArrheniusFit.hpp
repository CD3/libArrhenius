#ifndef Fitting_ArrheniusFit_hpp
#define Fitting_ArrheniusFit_hpp

/** @file ArrheniusFit.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/29/17
  */

#include "ArrheniusFitBase.hpp"

namespace libArrhenius {

struct MinimizeLogAStdDevAndScalingFactors {};
struct EffectiveExposureLinearRegression {};
struct ConstantTempeLinearRegression {};

/** @class ArrheniusFit
  * @brief 
  * @author C.D. Clark III
  */
template<typename Real, typename Method = MinimizeLogAStdDevAndScalingFactors>
class ArrheniusFit {};

}

// include specific implementations here as they
// won't work if the user tries to include them directly
#include "./detail/MinimizeLogAStdDevAndScalingFactors.hpp"

#endif // include protector
