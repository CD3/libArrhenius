#ifndef Fitting_detail_ConstantTemperatureLinearRegression_hpp
#define Fitting_detail_ConstantTemperatureLinearRegression_hpp

/** @file ConstantTemperatureLinearRegression.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 07/07/17
  */

#include <eigen3/Eigen/Dense>
#include "Utils/LinearRegression.hpp"

namespace libArrhenius {

/** @class ArrheniusFit<Real,ConstantTemperatureLinearRegression>
  * @brief Implements the standard constant-temperature linear regression method.
  * @author C.D. Clark III
  */
template<typename Real>
class ArrheniusFit<Real,ConstantTemperatureLinearRegression> : public ArrheniusFitBase<Real>
{
  public:
    ArrheniusFit() {};
    virtual ~ArrheniusFit() {};

    typedef typename ArrheniusFitBase<Real>::Return Return;

    Return
    exec() const
    {
      Return ret;
      ArrheniusIntegral<Real> integrator;
      std::vector<Real*> const &t = this->t;
      std::vector<Real*> const &T = this->T;
      std::vector<size_t> const &N = this->N;


      // construct effective exposure parameters for each profile
      BOOST_LOG_TRIVIAL(trace) << "Determining peak temperature and exposure durations.";
      Eigen::Matrix<Real,Eigen::Dynamic,1> logteff(N.size()),invTeff(N.size());
      for(int i = 0; i < N.size(); i++)
      {
        Real Tmax = *std::max_element( T[i], T[i]+N[i] );
        Real Tmin = *std::min_element( T[i], T[i]+N[i] );
        invTeff[i] = 1 / Tmax;

        // get the time above 50% temp rise
        logteff[i] = 0;
        for(int j = 1; j < N[i]; j++)
        {
          if( (T[i][j] + T[i][j-1])/2 > (Tmax+Tmin)/2 )
            logteff[i] += t[i][j] - t[i][j-1];
        }
        logteff[i] = log(logteff[i]);

        BOOST_LOG_TRIVIAL(trace) << "peak T: "<< 1/invTeff[i];
        BOOST_LOG_TRIVIAL(trace) << "tau: "<< exp(logteff[i]);
      }



      BOOST_LOG_TRIVIAL(trace) << "Performing linear regression.";
      // now perform linear regression with effective parameters
      auto linreg = RUC::LinearRegression( invTeff, logteff );

      ret.A = exp(-linreg[0]);
      ret.Ea = linreg[1]*Constants::MKS::R;


      return ret;
      
    }

  protected:
};

}

#endif // include protector
