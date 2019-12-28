#include "catch.hpp"
#include "fakeit.hpp"

#include <vector>

#include <libArrhenius/Integration/ArrheniusIntegral.hpp>
#include <libArrhenius/Fitting/ArrheniusFit.hpp>
#include <libArrhenius/ThresholdCalculator.hpp>

#include<boost/optional/optional_io.hpp>

#include <boost/multiprecision/cpp_dec_float.hpp>
using namespace boost::multiprecision;

using namespace libArrhenius;

typedef cpp_dec_float_100 DataType;

TEST_CASE( "ArrheniusFitter Usage (clark method)", "[usage]" ) {

  std::vector<double> taus = { 0.001, 0.01, 0.1, 1.0, 10.0 };
  std::vector<std::shared_ptr<DataType>> ts,Ts;
  std::vector<size_t> Ns;

  double A = 3.1e99;
  double Ea = 6.28e5;

  ThresholdCalculator< ArrheniusIntegral<DataType> > calc(A,Ea);
  ArrheniusFit< DataType > fit;

  for( auto tau : taus )
  {
    double dt = tau / 20;
    double total_time = 4*tau;
    size_t N = total_time / dt;

    Ns.push_back( N );
    ts.push_back( std::shared_ptr<DataType>( new DataType[N], [](DataType* p){delete[] p;} ) );
    Ts.push_back( std::shared_ptr<DataType>( new DataType[N], [](DataType* p){delete[] p;} ) );
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

TEST_CASE( "ArrheniusFitter Usage (effective exposures method)", "[usage]" ) {

  std::vector<double> taus = { 0.001, 0.01, 0.1, 1.0, 10.0 };
  std::vector<std::shared_ptr<DataType>> ts,Ts;
  std::vector<size_t> Ns;

  double A = 3.1e99;
  double Ea = 6.28e5;

  ThresholdCalculator< ArrheniusIntegral<DataType> > calc(A,Ea);
  ArrheniusFit< DataType, EffectiveExposuresLinearRegression > fit;

  for( auto tau : taus )
  {
    double dt = tau / 20;
    double total_time = 4*tau;
    size_t N = total_time / dt;

    Ns.push_back( N );
    ts.push_back( std::shared_ptr<DataType>( new DataType[N], [](DataType* p){delete[] p;} ) );
    Ts.push_back( std::shared_ptr<DataType>( new DataType[N], [](DataType* p){delete[] p;} ) );
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

  CHECK( static_cast<double>(ret.A.get()) == Approx(3.1e99).epsilon(0.2) ); // within 20%
  CHECK( static_cast<double>(ret.Ea.get()) == Approx(6.28e5).epsilon(0.1) ); // within 10%

  

}

TEST_CASE( "ArrheniusFitter Usage (standard method)", "[usage]" ) {

  std::vector<double> taus = { 0.001, 0.01, 0.1, 1.0, 10.0 };
  std::vector<std::shared_ptr<DataType>> ts,Ts;
  std::vector<size_t> Ns;

  double A = 3.1e99;
  double Ea = 6.28e5;

  ThresholdCalculator< ArrheniusIntegral<DataType> > calc(A,Ea);
  ArrheniusFit< DataType, ConstantTemperatureLinearRegression > fit;

  for( auto tau : taus )
  {
    double dt = tau / 200;
    double total_time = 4*tau;
    size_t N = total_time / dt;

    Ns.push_back( N );
    ts.push_back( std::shared_ptr<DataType>( new DataType[N], [](DataType* p){delete[] p;} ) );
    Ts.push_back( std::shared_ptr<DataType>( new DataType[N], [](DataType* p){delete[] p;} ) );
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

  CHECK( static_cast<double>(ret.A.get()) == Approx(3.1e99).epsilon(0.01) );
  CHECK( static_cast<double>(ret.Ea.get()) == Approx(6.28e5) );

  

}

TEST_CASE( "ArrheniusFitterInterface Usage", "[usage]" ) {

  std::vector<double> taus = { 0.001, 0.01, 0.1, 1.0, 10.0 };
  std::vector<std::shared_ptr<DataType>> ts,Ts;
  std::vector<size_t> Ns;

  double A = 3.1e99;
  double Ea = 6.28e5;

  ThresholdCalculator< ArrheniusIntegral<DataType> > calc(A,Ea);
  std::shared_ptr< ArrheniusFitInterface< DataType > > fit( new ArrheniusFit<DataType>() );

  for( auto tau : taus )
  {
    double dt = tau / 20;
    double total_time = 4*tau;
    size_t N = total_time / dt;

    Ns.push_back( N );
    ts.push_back( std::shared_ptr<DataType>( new DataType[N], [](DataType* p){delete[] p;} ) );
    Ts.push_back( std::shared_ptr<DataType>( new DataType[N], [](DataType* p){delete[] p;} ) );
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

    fit->addProfile( Ns[j], ts[j].get(), Ts[j].get() );
  }

  auto ret = fit->exec();

  CHECK( static_cast<double>(ret.A.get()) == Approx(3.1e99) );
  CHECK( static_cast<double>(ret.Ea.get()) == Approx(6.28e5) );

  

}

