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
    size_t parallel_threshold = 4096;

  public:

    template<typename T>
    void setA( T A_ ) { A = A_; }

    template<typename T>
    void setEa( T Ea_ ) { Ea = Ea_; }

    void setParallelThreshold( size_t t ) { parallel_threshold = t; }


  protected:
};

#endif // include protector
