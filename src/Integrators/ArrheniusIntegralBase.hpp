#ifndef Integrators_ArrheniusIntegralBase_hpp
#define Integrators_ArrheniusIntegralBase_hpp

/** @file ArrheniusIntegralBase.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/27/17
  */

/** @class ArrheniusIntegralBase
  * @brief Base class for common Arrhenius integral data.
  * @author C.D. Clark III
  */
template<typename Real>
class ArrheniusIntegralBase
{
  protected:
    Real A, Ea;

  public:

    template<typename T>
    void setA( T A_ ) { A = A_; }

    template<typename T>
    void setEa( T Ea_ ) { Ea = Ea_; }


  protected:
};

#endif // include protector
