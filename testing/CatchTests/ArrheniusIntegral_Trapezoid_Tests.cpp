#include "catch.hpp"
#include "fakeit.hpp"

#include <vector>
#include "Integrators/ArrheniusIntegral.hpp"

using namespace libArrhenius;
using namespace libArrhenius::Constants;


TEST_CASE( "ArrheniusIntegral Usage", "[trapezoid]" ) {

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

  ArrheniusIntegral<double> Arr(A,Ea);
  
  Omega = Arr(N,t.data(),T.data());
  CHECK( Omega == Approx(A*exp(-Ea/(MKS::R*410))*tau + A*exp(-Ea/(MKS::R*310))*3*tau) );


  A = 2.00e30;
  Ea = 2.00e5;

  Arr.setA(A);
  Arr.setEa(Ea);
  
  Omega = Arr(N,t.data(),T.data());

  CHECK( Omega == Approx(A*exp(-Ea/(MKS::R*410))*tau + A*exp(-Ea/(MKS::R*310))*3*tau).epsilon(0.0001) );

  // this will result in a number too small for double
  A = 2.00e30;
  Ea = 2.00e10;

  Arr.setA(A);
  Arr.setEa(Ea);
  
  Omega = Arr(N,t.data(),T.data());

  std::cout << "Omega: " << Omega << std::endl;

  // this will result in a number too large for double
  A = 2.00e300;
  Ea = -2.00e5;

  Arr.setA(A);
  Arr.setEa(Ea);
  
  Omega = Arr(N,t.data(),T.data());

  std::cout << "Omega: " << Omega << std::endl;


}

#include <boost/multiprecision/cpp_dec_float.hpp>
using namespace boost::multiprecision;

//TEST_CASE( "ArrheniusIntegral With Boost.Multiprecision", "[trapezoid]" ) {

  //// doesn't seem to work on Ubuntu with boost 1.57
  //// complains about the convert_to<>() calls...
  //cpp_dec_float_100 tau = 2;
  //cpp_dec_float_100 dt = tau / 20;
  //size_t N = (4*tau / dt).convert_to<size_t>();
  //std::vector<cpp_dec_float_100> t(N), T(N);

  //for( size_t i = 0; i < t.size(); i++ )
  //{
    //t[i] = dt*i;
    //T[i] = 310;
    //if( t[i] > tau/2 )
      //T[i] = 100 + 310;
    //if( t[i] > tau + tau/2 )
      //T[i] = 310;
  //}

  //cpp_dec_float_100 A, Ea, Omega;

  //A = 3.1e99;
  //Ea = 6.28e5;

  //ArrheniusIntegral<cpp_dec_float_100> Arr(A,Ea);
  
  //Omega = Arr(N,t.data(),T.data());
  //CHECK( Omega.convert_to<double>() == Approx((A*exp(-Ea/(MKS::R*410))*tau + A*exp(-Ea/(MKS::R*310))*3*tau).convert_to<double>()));


  //A = 2.00e30;
  //Ea = 2.00e5;

  //Arr.setA(A);
  //Arr.setEa(Ea);
  
  //Omega = Arr(N,t.data(),T.data());

  //CHECK( Omega.convert_to<double>() == Approx( (A*exp(-Ea/(MKS::R*410))*tau + A*exp(-Ea/(MKS::R*310))*3*tau).convert_to<double>()).epsilon(0.001) );

  //A = 2.00e30;
  //Ea = 2.00e10;

  //Arr.setA(A);
  //Arr.setEa(Ea);
  
  //Omega = Arr(N,t.data(),T.data());

  //std::cout << "Omega: " << Omega << std::endl;

  //// this will result in a number too large for double
  //A = 2.00e300;
  //Ea = -2.00e5;

  //Arr.setA(A);
  //Arr.setEa(Ea);
  
  //Omega = Arr(N,t.data(),T.data());

  //std::cout << "Omega: " << Omega << std::endl;
//}

