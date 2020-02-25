#include "catch.hpp"

#include <vector>
#include <iostream>

#include <libArrhenius/Integration/ArrheniusIntegral.hpp>
TEST_CASE( "Simple Integral Evaluation", "[examples]" ) {

  int N = 120;
  std::vector<double> t(N),T(N);
  double dt = 0.01;
  for(int i = 0; i < N; ++i)
  {
    t[i] = dt*i;
    T[i] = 310;
    if( i > 10 )
      T[i] += 10;
    if( i > N-10 )
      T[i] -= 10;

    // |     +-----------+
    // |     |           |
    // |     |           |
    // | ----+           +----
    // |______________________________
  }

  libArrhenius::ArrheniusIntegral<double> ArrIntegral(3.1e99,6.28e5);


  double Omega = ArrIntegral(N,t.data(),T.data());

  std::cout << "Omega: " << Omega << std::endl;


  


  

}

