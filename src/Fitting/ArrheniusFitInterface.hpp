#ifndef Fitting_ArrheniusFitInterface_hpp
#define Fitting_ArrheniusFitInterface_hpp

/** @file ArrheniusFitInterface.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 07/06/17
  */

namespace libArrhenius {

/** @class ArrheniusFitInterface
  * @brief 
  * @author C.D. Clark III
  */
template<typename Real>
class ArrheniusFitInterface
{
  public:

  struct Return
  {
    boost::optional<Real> A, Ea;
  };

  virtual void addProfile( size_t N_, Real* t_, Real* T_ ) = 0;
  virtual void clear() = 0;
  virtual Return exec() const = 0;


  protected:
};

}


#endif // include protector
