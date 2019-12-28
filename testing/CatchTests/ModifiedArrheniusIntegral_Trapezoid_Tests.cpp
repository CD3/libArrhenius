#include "catch.hpp"
#include "fakeit.hpp"

#include <vector>
#include <libArrhenius/Integration/ModifiedArrheniusIntegral.hpp>

using namespace libArrhenius;
using namespace libArrhenius::Constants;


TEST_CASE( "ModifiedArrheniusIntegral With n=0", "[trapezoid]" ) {

  double tau = 2;
  double dt = tau / 20;
  size_t N = 4*tau / dt;
  std::vector<double> t(N), T(N);

  for( size_t i = 0; i < t.size(); i++ )
  {
    t[i] = dt*i;
    T[i] = 310;
    if( t[i] > tau/2 )
      T[i] = 100 + 310;
    if( t[i] > tau + tau/2 )
      T[i] = 310;
  }

  double A, Ea, Omega;

  A = 3.1e99;
  Ea = 6.28e5;

  ModifiedArrheniusIntegral<double> Arr(A,Ea,0);
  
  Omega = Arr(N,t.data(),T.data());
  CHECK( Omega == Approx(A*exp(-Ea/(MKS::R*410))*tau + A*exp(-Ea/(MKS::R*310))*3*tau) );

}

