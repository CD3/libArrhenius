#ifndef Fitting_MinimizeLogAVarianceAndScalingFactors_hpp
#define Fitting_MinimizeLogAVarianceAndScalingFactors_hpp

/** @file MinimizeLogAVarianceAndScalingFactors.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/28/17
  */

#include <utility>


#include <boost/math/tools/minima.hpp>
using boost::math::tools::brent_find_minima;

#include <boost/math/tools/roots.hpp>
using boost::math::tools::bracket_and_solve_root;
using boost::math::tools::eps_tolerance;

#include "ThresholdCalculator.hpp"


namespace libArrhenius {

/** @class MinimizeLogAVarianceAndScalingFactors
  * @brief 
  * @author C.D. Clark III
  */
template<typename Real>
class ArrheniusFit<Real,MinimizeLogAVarianceAndScalingFactors> : public ArrheniusFitBase<Real>
{
  protected:
    
  public:
    ArrheniusFit() {};
    virtual ~ArrheniusFit() {};

    typedef typename ArrheniusFitBase<Real>::Return Return;

    Return
    exec() const
    {
      // find Ea that minimizes the variance in corresponding A values.
      // then find A that minimizes scaling factor error

      Return ret;
      ArrheniusIntegral<Real> integrator;
      ThresholdCalculator<decltype(integrator)> calc;
      std::vector<Real*> const &t = this->t;
      std::vector<Real*> const &T = this->T;
      std::vector<size_t> const &N = this->N;

        

      // search for Ea
      
      // the cost function for choosing Ea
      // calculates and returns the sum of squared deviations.
      auto Ea_cost = [&](Real Ea){

        // cost is equal to the sum of squared deviations (i.e. proportional to variance)
        integrator.setEa( Ea );
        integrator.setA(1);

        std::vector<Real> logAs(N.size());
        for(size_t i = 0; i < N.size(); ++i)
          logAs[i] = -log( integrator(N[i], t[i], T[i]) );

        // calculate the mean
        Real mean = 0;
        for(size_t i = 0; i < N.size(); ++i)
          mean += logAs[i];
        mean /= N.size();

        // calculate the sum of squared deviations
        Real devs = 0;
        for(size_t i = 0; i < N.size(); ++i)
          devs += (logAs[i] - mean)*(logAs[i] - mean);


       return devs;
      };

      // We need to get a range for Ea before we can run the minimization
      // function. So, we'll do a quick scan for the minimum.
      Real Ea_lb = 0;
      Real Ea_ub = 0;

      int prec = std::numeric_limits<Real>::digits - 3;
      boost::uintmax_t maxit;

      {
        // If Ea is too large, then the Arrhenius integral will
        // return zero. The point at which this happens depends on the
        // data type being used. Basically, higher-precision types can
        // evaluate the integral at larger Ea's before reaching zero.
        // So, we know Ea can't be larger than the smallest value that
        // gives zero for the integral. This gives us an initial upper bound on Ea.
        for( int i = 0; i < N.size(); i++ )
        {
          eps_tolerance<Real> tol( prec );
          maxit = 100;
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

        // Now do a quick scan for the minimum.
        // We'll calculate the cost at every half decade for Ea.
        int exponent = log10(Ea_ub);
        Real last_cost;
        for(Real e = 0; e < exponent; e += 0.5)
        {
          Ea_ub = pow(10,e);
          Real cost = Ea_cost(Ea_ub);
          if( e == 0 || cost < last_cost )
            Ea_lb = Ea_ub;
          if( e > 0 && cost > last_cost)
            break;
          last_cost = cost;
        }
      }

      maxit = 1000;
      auto Ea_min = brent_find_minima( Ea_cost, Ea_lb, Ea_ub, prec );
      ret.Ea = Ea_min.first;

      // search for A

      // the cost function for choosing A
      // calculates and returns the sum of squared deviations.
      auto A_cost = [&](Real A){

        // cost is equal to the sum of squared deviations (i.e. proportional to variance)
        calc.setEa( ret.Ea.get() );
        calc.setA( A );

        std::vector<Real> thresholds(N.size());
        for(size_t i = 0; i < N.size(); ++i)
          thresholds[i] = calc(N[i], t[i], T[i]);

        // calculate the sum of squared deviations
        Real devs = 0;
        for(size_t i = 0; i < N.size(); ++i)
          devs += (thresholds[i] - 1)*(thresholds[i] - 1);

        return devs;
      };

      // get the range to search for A
      std::vector<Real> As(N.size());
      for(size_t i = 0; i < N.size(); ++i)
        As[i] = 1/integrator(N[i], t[i], T[i]);
      Real A_lb = *std::min_element(As.begin(), As.end());
      Real A_ub = *std::max_element(As.begin(), As.end());


      maxit = 1000;
      auto A_min = brent_find_minima( A_cost, A_lb, A_ub, prec );
      ret.A = A_min.first;

      // OR....
      // find zero for log of average threshold
      // both seem to work.
      //{
        //eps_tolerance<Real> tol( prec );
        //maxit = 100;
        //Real guess = 1/integrator(N[0],t[0],T[0]);
        //Real factor = 2;
        //auto A_range = bracket_and_solve_root( [&](Real A){
            //calc.setEa(ret.Ea.get());
            //calc.setA(A);
            //Real sum = 0;
            //for( int i = 0; i < N.size(); ++i )
              //sum += calc(N[i], t[i], T[i]);
            //sum /= N.size();

            //return static_cast<Real>(log(sum));}, guess, factor, false, tol, maxit );

      //}

      return ret;
    }


  protected:
};

}

#endif // include protector
