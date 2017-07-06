#include "catch.hpp"
#include "fakeit.hpp"

#include <vector>
#include "Constants.hpp"
#include "Integration/ArrheniusIntegral.hpp"
#include "ThresholdCalculator.hpp"

using namespace libArrhenius;
using namespace libArrhenius::Constants;


TEST_CASE( "ThresholdCalculator Usage", "[usage]" ) {

  double tau = 2;
  double dt = tau / 20;
  size_t N = 4*tau / dt;
  std::vector<double> t(N), T(N);

  for( size_t i = 0; i < t.size(); i++ )
  {
    t[i] = dt*i;
    T[i] = 310;
    if( t[i] > tau/2 )
      T[i] = 10 + 310;
    if( t[i] > tau + tau/2 )
      T[i] = 310;
  }

  double A = 3.1e99;
  double Ea = 6.28e5;


  ThresholdCalculator< ArrheniusIntegral<double> > calc(A,Ea);
  
  double threshold = calc(N,t.data(),T.data());

  CHECK( threshold == Approx( (Ea/(MKS::R*log(A*tau)) - 310) / 10) );
}

