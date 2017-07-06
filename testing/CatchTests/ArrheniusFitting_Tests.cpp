#include "catch.hpp"
#include "fakeit.hpp"

#include <vector>

#include "Integration/ArrheniusIntegral.hpp"
#include "Fitting/ArrheniusFit.hpp"
#include "ThresholdCalculator.hpp"

#include<boost/optional/optional_io.hpp>

#include <boost/multiprecision/cpp_dec_float.hpp>
using namespace boost::multiprecision;

using namespace libArrhenius;

typedef cpp_dec_float_100 DataType;

TEST_CASE( "ArrheniusFitter Usage", "[usage]" ) {

  std::vector<double> taus = { 0.001, 0.01, 0.1, 1.0, 10.0 };
  std::vector<std::shared_ptr<DataType>> ts,Ts;
  std::vector<size_t> Ns;

  double A = 3.1e99;
  double Ea = 6.28e5;

  ThresholdCalculator< ArrheniusIntegral<DataType> > calc(A,Ea);
  ArrheniusFit< DataType > fit;

  for( auto tau : taus )
  {
    double dt = tau / 2000;
    double total_time = 4*tau;
    size_t N = total_time / dt;
    std::cout << "N: " << N << std::endl;

    Ns.push_back( N );
    ts.push_back( std::shared_ptr<DataType>( new DataType[N] ) );
    Ts.push_back( std::shared_ptr<DataType>( new DataType[N] ) );
    int j = Ns.size()-1;

    for( size_t i = 0; i < Ns[j]; i++ )
    {
      ts[j].get()[i] = dt*i;
      Ts[j].get()[i] = 310;
      if( ts[j].get()[i] > tau/2 )
        Ts[j].get()[i] = 10 + 310;
      if( ts[j].get()[i] > tau + tau/2 )
        Ts[j].get()[i] = 310;
    }

    auto Threshold = calc(Ns[j],ts[j].get(),Ts[j].get());
    for( size_t i = 0; i < Ns[j]; i++ )
    {
      Ts[j].get()[i] = Threshold*(Ts[j].get()[i] - Ts[j].get()[0]) + Ts[j].get()[0];
    }

    auto Omega = calc.Omega(Ns[j],ts[j].get(),Ts[j].get());
    CHECK( static_cast<double>(Omega) == Approx(1) );

    fit.addProfile( Ns[j], ts[j].get(), Ts[j].get() );
  }

  auto ret = fit.exec();

  CHECK( static_cast<double>(ret.A.get()) == Approx(3.1e99) );
  CHECK( static_cast<double>(ret.Ea.get()) == Approx(6.28e5) );

  

}

