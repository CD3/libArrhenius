#ifndef Fitting_ArrheniusFitBase_hpp
#define Fitting_ArrheniusFitBase_hpp

/** @file ArrheniusFitBase.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/28/17
  */
#include<boost/optional.hpp>
#include<boost/log/trivial.hpp>
#include"ArrheniusFitInterface.hpp"

namespace libArrhenius {

/** @class ArrheniusFitBase
  * @brief 
  * @author C.D. Clark III
  */
template<typename Real>
class ArrheniusFitBase : public ArrheniusFitInterface<Real>
{
  protected:
    std::vector<Real*> t,T;
    std::vector<size_t> N;
    boost::optional<Real> minEa, maxEa;
    
  public:
    ArrheniusFitBase (){};
    virtual ~ArrheniusFitBase (){};

    void addProfile( size_t N_, Real* t_, Real* T_ )
    {
      t.push_back(t_);
      T.push_back(T_);
      N.push_back(N_);
    }

    void clear()
    {
      t.clear();
      T.clear();
      N.clear();
    }

    void setMinEa( Real minEa_ ) { minEa = minEa_; }
    void setMaxEa( Real maxEa_ ) { maxEa = maxEa_; }
    Real getMinEa( ) const { return minEa.get(); }
    Real getMaxEa( ) const { return maxEa.get(); }

  protected:
};

}

#endif // include protector
