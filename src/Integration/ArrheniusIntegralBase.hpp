#ifndef Integration_ArrheniusIntegralBase_hpp
#define Integration_ArrheniusIntegralBase_hpp

/** @file ArrheniusIntegralBase.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/27/17
  */
#include "Constants.hpp"

namespace libArrhenius {

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
    Real getA( ) { return A; }

    template<typename T>
    void setEa( T Ea_ ) { Ea = Ea_; }
    void getEa( ) const { return Ea; }

    void setParallelThreshold( size_t t ) { parallel_threshold = t; }
    size_t getParallelThreshold( ) const { return parallel_threshold; }

    Real getCriticalTemperature() const { return Ea / Constants::MKS::R / log(A); }

    Real rate( Real T ) const { return A*exp( -Ea / Constants::MKS::R /T ); }

  protected:
};

}

#endif // include protector
