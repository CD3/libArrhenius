#ifndef Fitting_MinimizeLogAStdDevAndScalingFactors_hpp
#define Fitting_MinimizeLogAStdDevAndScalingFactors_hpp

/** @file MinimizeLogAStdDevAndScalingFactors.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/28/17
  */

#include "ArrheniusFitBase.hpp"
#include "../ThresholdCalculator.hpp"

namespace libArrhenius {

/** @class MinimizeLogAStdDevAndScalingFactors
  * @brief 
  * @author C.D. Clark III
  */
template<typename Real, typename IntegatorMethod = Trapezoid >
class MinimizeLogAStdDevAndScalingFactors : ArrheniusFitBase<Real>
{
  protected:
    
  public:
    MinimizeLogAStdDevAndScalingFactors () {};
    virtual ~MinimizeLogAStdDevAndScalingFactors () {};




  protected:
};

}

#endif // include protector
