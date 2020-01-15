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

#include "../../ThresholdCalculator.hpp"


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
      BOOST_LOG_TRIVIAL(trace) << "MinimizeLogAVarianceAndScalignFactors: Executing Fit";
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

      BOOST_LOG_TRIVIAL(trace) << "Searching for upper bound on Ea";
      int prec = std::numeric_limits<Real>::digits - 3;
      boost::uintmax_t maxit;

      {
        // If Ea is too large, then the Arrhenius integral will
        // return zero. The point at which this happens depends on the
        // data type being used. Basically, higher-precision types can
        // evaluate the integral at larger Ea's before reaching zero.
        // So, we know Ea can't be larger than the smallest value that
        // gives zero for the integral. This gives us an initial upper bound on Ea.
        bool found_one = false;
        for( size_t i = 0; i < N.size(); i++ )
        {
          eps_tolerance<Real> tol( prec );
          maxit = 100;
          Real guess = 1e2; // a place to start
          Real factor = 2;  // multiplication factor to use when searching for upper bound
          try {
          auto Ea_ub_range = bracket_and_solve_root( [&](Real Ea){
              integrator.setEa(Ea);
              integrator.setA(1);
              return integrator(N[i],t[i],T[i]);}, guess, factor, false, tol, maxit );

              // use the smallest Ea for the upper bound.
              if( !found_one || Ea_ub_range.first < Ea_ub )
                Ea_ub = Ea_ub_range.first;
              found_one = true;
          } catch( ... ) {
            BOOST_LOG_TRIVIAL(warning) <<"WARNING: Could not determine an upper bound on Ea from thermal profile number " << i << ". Skipping";
          }
        }
        if( !found_one )
        {
          BOOST_LOG_TRIVIAL(fatal)<<"ERROR: Could not determine an upper-bound on Ea from any of the thermal profiles.";
          throw std::runtime_error( "ERROR: Could not determine an upper-bound on Ea from any of the thermal profiles.");
        }

        

        // Now do a quick scan for the minimum.
        // We'll calculate the cost at every half decade for Ea.
        BOOST_LOG_TRIVIAL(trace) << "Scanning for minimum (rough estimate)";

        
        
        while( Ea_cost(Ea_ub) == 0 )
          Ea_ub /= 2;

        // scan Ea from 1 to Ea_ub on a log-scale
        Real min_lnEa, max_lnEa, d_lnEa;
        min_lnEa = 0;
        max_lnEa = log(Ea_ub);
        // check if we have limits configured. if so,
        // we should use them if they are more restrictive.
        if( this->minEa && log(this->minEa.get()) > min_lnEa )
          min_lnEa = log(this->minEa.get());
        if( this->maxEa && log(this->maxEa.get()) < max_lnEa )
          max_lnEa = log(this->maxEa.get());
        // discretize
        int num  = static_cast<int>((max_lnEa - min_lnEa) / 0.5); // half log spacing
        d_lnEa = (max_lnEa - min_lnEa) / (num - 1);
        int i_of_min = 0;
        Real min_cost = Ea_cost(exp(min_lnEa));
        for(int i = 1; i < num; ++i)
        {
          Ea_ub = exp(min_lnEa + i*d_lnEa);
          Real cost = Ea_cost(Ea_ub);
          if( cost < min_cost )
          {
            i_of_min = i;
            min_cost = cost;
          }
        }
        Ea_lb = exp(min_lnEa + (i_of_min-1)*d_lnEa);
        Ea_ub = exp(min_lnEa + (i_of_min+1)*d_lnEa);
        BOOST_LOG_TRIVIAL(trace) << "Minimum between " << Ea_lb << " and " << Ea_ub;
      }

      maxit = 1000;
      BOOST_LOG_TRIVIAL(trace) << "Searching for Ea with Cost minimization";
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
      BOOST_LOG_TRIVIAL(trace) << "Searching for A with Cost minimization";
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
