#ifndef ThresholdCalculator_hpp
#define ThresholdCalculator_hpp

#include <utility>

#include <boost/math/tools/roots.hpp>
using boost::math::tools::bracket_and_solve_root;
using boost::math::tools::eps_tolerance;

/** @file ThresholdCalculator.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 06/26/17
  */

/** @class ThresholdCalculator
  * @brief 
  * @author C.D. Clark III
  */
// this allows the specialization below to detuce Real and Method types for integrators.
template< class Integrator >
class ThresholdCalculator {};

template< template<typename,typename> class Integrator, typename Real, typename Method>
class ThresholdCalculator<Integrator<Real,Method>> : public Integrator<Real,Method>
  // by inheriting from the integrator, we can configure it directly through
  // the threshold calculator without writing wrapper functions.
{
  public:
    template<typename ...Args>
    ThresholdCalculator(Args&&... args)
    :Integrator<Real,Method>(std::forward<Args>(args)...)
    {
    }

    virtual ~ThresholdCalculator () {};

    Real operator()(size_t N, Real const *t, Real const *T)
    {
      Real *dT = new Real[N];
      Real *TT = new Real[N];

      for(size_t i = 0; i < N; i++)
        dT[i] = T[i] - T[0];

      // the function that will be rooted
      // should take a single argument and return
      // something.
      auto f = [&](Real x){

        // create the temperature profile that will be evaluated.
        for(size_t i = 0; i < N; i++)
          TT[i] = T[0] + x*dT[i];
        

        // calculate damage parameter
        Real Omega = Integrator<Real,Method>::operator()(N,t,TT);

        // damage will be between zero and infinity. we are looking for
        // the value of x that will give Omega = 1, so return log of Omega.

        return log(Omega);
      };

      eps_tolerance<double> tol( std::numeric_limits<Real>::digits - 3 ); // maximum precision we can reasonably expect to achieve.
      boost::uintmax_t it = 100;  // maximum number of iterations to all the algorithm to try.

      // CAREFULE: make sure initial guess is not an integer!
      Real guess = 1.0;

      if( f(0) > 0 )
        guess = -1.0;

      auto min_max = bracket_and_solve_root(f, guess, 2., true, tol, it);

      // clean up our memory
      delete[] dT;
      delete[] TT;

      return (min_max.first + min_max.second)/2;
      
    }

  protected:
};

#endif // include protector
