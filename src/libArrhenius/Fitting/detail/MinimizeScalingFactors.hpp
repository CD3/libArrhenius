#pragma once

/** @file MinimizeScalingFactors.hpp
 * @brief
 * @author C.D. Clark III
 * @date 10/13/20
 */



#include <utility>


#include <boost/math/tools/minima.hpp>
using boost::math::tools::brent_find_minima;

#include <boost/math/tools/roots.hpp>
using boost::math::tools::bracket_and_solve_root;
using boost::math::tools::eps_tolerance;

#include "../../ThresholdCalculator.hpp"


namespace libArrhenius {

  /** @class MinimizeScalingFactors
   * @brief
   * @author C.D. Clark III
   */
  template<typename Real>
    class ArrheniusFit<Real,MinimizeScalingFactors> : public ArrheniusFitBase<Real>
    {
      protected:

      public:
        ArrheniusFit() {};
        virtual ~ArrheniusFit() {};

        typedef typename ArrheniusFitBase<Real>::Return Return;

        Return
          exec() const
          {
            BOOST_LOG_TRIVIAL(trace) << "MinimizeScalignFactors: Executing Fit";
            // find Ea and A that minimizes scaling factor error

            Return ret;
            ArrheniusIntegral<Real> integrator;
            ThresholdCalculator<decltype(integrator)> calc;
            std::vector<Real*> const &t = this->t;
            std::vector<Real*> const &T = this->T;
            std::vector<size_t> const &N = this->N;


            // We need to get a range for Ea and A before we can run the minimization
            // function. So, we'll do a quick scan for the minimum.
            // If the caller has specified a bound use it.
            // otherwise, calculate one
            Real Ea_lb = 5e5;
            Real Ea_ub = 9e5;
            Real A_lb = 1e99;
            Real A_ub = 5e99;

            /* int prec; */
            /* boost::uintmax_t maxit; */

            /* prec = std::numeric_limits<Real>::digits - 3; */
            /* maxit = 1000; */

            // the cost function calculates and returns the sum of squared deviations of the scaling factors from 1
            auto cost = [&](Real Ea, Real A){
              // cost is equal to the sum of squared deviations (i.e. proportional to variance)
              calc.setEa( Ea );
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

            /* maxit = 1000; */
            BOOST_LOG_TRIVIAL(trace) << "Searching for Ea and A with Cost minimization in region Ea = [" << Ea_lb << "," << Ea_ub <<"], A = [" << A_lb << "," << A_ub << "]";
            auto dEa = (Ea_ub - Ea_lb)/100;
            auto dA = (A_ub - A_lb)/10;
            for(int i = 0; i < 10; ++i)
            {
              auto Ea = Ea_lb + i*dEa;
              for(int j = 0; j < 100; ++j)
              {
                auto A = A_lb + j*dA;
                try{
                  auto c = cost(Ea,A);
                std::cout << Ea << " " << A << " " << c << "\n";
                }catch(...){}

              }
              std::cout << "\n";
            }
            /* auto A_min = brent_find_minima( cost, A_lb, A_ub, prec ); */
            /* ret.A = A_min.first; */


            return ret;

          }


      protected:
    };

}

