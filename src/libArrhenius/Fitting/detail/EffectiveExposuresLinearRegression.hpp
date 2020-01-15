#ifndef Fitting_detail_EffectiveExposuresLinearRegression_hpp
#define Fitting_detail_EffectiveExposuresLinearRegression_hpp

/** @file EffectiveExposuresLinearRegression.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 07/07/17
  */

#include <Eigen/Dense>
#include "../../Utils/LinearRegression.hpp"

namespace libArrhenius {

/** @class ArrheniusFit<Real,EffectiveExposuresLinearRegression>
  * @brief Implements the "effective exposure" method from "Arrhenius Model Thermal Damage Coefficients for Birefringence Loss in Rabbit Myocardium" Pearse, Raghavan, and Thomsen (2003).
  * @author C.D. Clark III
  */
template<typename Real>
class ArrheniusFit<Real,EffectiveExposuresLinearRegression> : public ArrheniusFitBase<Real>
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
      Eigen::Matrix<Real,Eigen::Dynamic,1> logteff(N.size()),invTeff(N.size());


      // Get a range for Ea to evaluate A over
      Real Ea_ub = 0;
      // If Ea is too large, then the Arrhenius integral will
      // return zero. The point at which this happens depends on the
      // data type being used. Basically, higher-precision types can
      // evaluate the integral at larger Ea's before reaching zero.
      // So, we know Ea can't be larger than the smallest value that
      // gives zero for the integral. This gives us an initial upper bound on Ea.
      for( size_t i = 0; i < N.size(); i++ )
      {
        int prec = std::numeric_limits<Real>::digits - 3;
        eps_tolerance<Real> tol( prec );
        boost::uintmax_t maxit = 100;
        Real guess = 1e2; // a place to start
        Real factor = 2;  // multiplication factor to use when searching for upper bound
        auto Ea_ub_range = bracket_and_solve_root( [&](Real Ea){
            integrator.setEa(Ea);
            integrator.setA(1);
            return integrator(N[i],t[i],T[i]);}, guess, factor, false, tol, maxit );
        // use the smallest Ea for the upper bound.
        if( i == 0 || Ea_ub_range.first < Ea_ub )
          Ea_ub = Ea_ub_range.first;
      }


      // compute a set of (Ea,log(A)) pairs
      for(size_t i = 0; i < N.size(); i++)
      {

        // We'll calculate (Ea,log(A)) pairs for every half decade
        int emin = 0;
        int emax = static_cast<int>(log10(Ea_ub));
        Real de = 0.1;
        int num = 1+static_cast<int>((emax - emin) / de);

        Eigen::Matrix<Real,Eigen::Dynamic,1> Eas(num),logAs(num);
        for(int j = 0; j < num; ++j)
        {
          Eas[j] = pow(10,emin + de*j);
          integrator.setEa(Eas[j]);
          integrator.setA(1);
          logAs[j] = -log( integrator(N[i], t[i], T[i]) );
        }
        auto linreg = RUC::LinearRegression(Eas,logAs);
        // linreg[0] is 'b',
        // linreg[1] is 'm' for the fit
        logteff[i] = -linreg[0];
        invTeff[i] = linreg[1]*Constants::MKS::R;
      }

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
