#include "catch.hpp"

#include <vector>

#include <libArrhenius/Integration/ArrheniusIntegral.hpp>

#include "fakeit.hpp"

using namespace libArrhenius;
using namespace libArrhenius::Constants;

TEST_CASE("ArrheniusIntegral Usage", "[integral]")
{
  double              tau = 2;
  double              dt  = tau / 20;
  size_t              N   = 4 * tau / dt;
  std::vector<double> t(N), T(N);

  for (size_t i = 0; i < t.size(); i++) {
    t[i] = dt * i;
    T[i] = 310;
    if (t[i] > tau / 2) T[i] = 100 + 310;
    if (t[i] > tau + tau / 2) T[i] = 310;
  }

  double A, Ea, Omega;

  A  = 3.1e99;
  Ea = 6.28e5;

  SECTION("Trapezoid")
  {
    ArrheniusIntegral<double> Arr(A, Ea);

    Omega = Arr(N, t.data(), T.data());
    CHECK(Omega == Approx(A * exp(-Ea / (MKS::R * 410)) * tau +
                          A * exp(-Ea / (MKS::R * 310)) * 3 * tau));

    A  = 2.00e30;
    Ea = 2.00e5;

    Arr.setA(A);
    Arr.setEa(Ea);

    Omega = Arr(N, t.data(), T.data());

    CHECK(Omega == Approx(A * exp(-Ea / (MKS::R * 410)) * tau +
                          A * exp(-Ea / (MKS::R * 310)) * 3 * tau)
                       .epsilon(0.0001));

    // this will result in a number too small for double
    A  = 2.00e30;
    Ea = 2.00e10;

    Arr.setA(A);
    Arr.setEa(Ea);

    Omega = Arr(N, t.data(), T.data());

    CHECK(Omega == 0);

    // this will result in a number too large for double
    A  = 2.00e300;
    Ea = -2.00e5;

    Arr.setA(A);
    Arr.setEa(Ea);

    Omega = Arr(N, t.data(), T.data());

    CHECK(Omega == std::numeric_limits<double>::infinity());
  }
}

TEST_CASE("ArrheniusIntegral Large Profile", "[integral]")
{
  double A, Ea, Omega;

  A  = 3.1e99;
  Ea = 6.28e5;

  ArrheniusIntegral<double> Arr(A, Ea);

  // create a data set that will trigger parallelization.
  double              tau = 2;
  double              dt  = tau / 200;
  size_t              N   = 1000;
  std::vector<double> t(N), T(N);

  SECTION("Constant Temperature")
  {
    for (size_t i = 0; i < t.size(); i++) {
      t[i] = dt * i;
      T[i] = 310;
      if (t[i] > tau / 2) T[i] = 100 + 310;
      if (t[i] > tau + tau / 2) T[i] = 310;
    }

    double Exact = A * exp(-Ea / (MKS::R * 410)) * tau + A * exp(-Ea / (MKS::R * 310)) * 3 * tau;

    SECTION("Trapezoid")
    {
      ArrheniusIntegral<double, Trapezoid> Arr(A, Ea);
      Omega = Arr(N, t.data(), T.data());
      CHECK(Omega == Approx(Exact));
    }
    SECTION("Exponential Integral")
    {
      ArrheniusIntegral<double, ExponentialIntegral> Arr(A, Ea);
      Omega = Arr(N, t.data(), T.data());
      CHECK(Omega == Approx(Exact));
    }
  }

  SECTION("Linear Temperature")
  {
    for (size_t i = 0; i < t.size(); i++) {
      t[i] = dt * i;
      T[i] = 310 + t[i];
    }

    double t0 = t[0];
    double t1 = t[t.size()-1];
    double T0 = T[0];
    double T1 = T[T.size()-1];
    double Exact = A*(t1-t0)/(T1-T0)*( T1*boost::math::expint(2,Ea/(MKS::R * T1)) - T0*boost::math::expint(2,Ea/(MKS::R * T0)) );

    SECTION("Trapezoid")
    {
      ArrheniusIntegral<double, Trapezoid> Arr(A, Ea);
      Omega = Arr(N, t.data(), T.data());
      CHECK(Omega == Approx(Exact));
    }
    SECTION("Exponential Integral")
    {
      ArrheniusIntegral<double, ExponentialIntegral> Arr(A, Ea);
      Omega = Arr(N, t.data(), T.data());
      CHECK(Omega == Approx(Exact));
    }
  }
}

#include <boost/multiprecision/cpp_dec_float.hpp>
using namespace boost::multiprecision;

TEST_CASE("ArrheniusIntegral With Boost.Multiprecision", "[integral]")
{
  // doesn't seem to work on Ubuntu with boost 1.57
  // complains about the convert_to<>() calls...
  cpp_dec_float_100              tau = 2;
  cpp_dec_float_100              dt  = tau / 20;
  size_t                         N   = (4 * tau / dt).convert_to<size_t>();
  std::vector<cpp_dec_float_100> t(N), T(N);

  for (size_t i = 0; i < t.size(); i++) {
    t[i] = dt * i;
    T[i] = 310;
    if (t[i] > tau / 2) T[i] = 100 + 310;
    if (t[i] > tau + tau / 2) T[i] = 310;
  }

  cpp_dec_float_100 A, Ea, Omega;

  A  = 3.1e99;
  Ea = 6.28e5;

  ArrheniusIntegral<cpp_dec_float_100> Arr(A, Ea);

  Omega = Arr(N, t.data(), T.data());
  CHECK(Omega.convert_to<double>() ==
        Approx((A * exp(-Ea / (MKS::R * 410)) * tau +
                A * exp(-Ea / (MKS::R * 310)) * 3 * tau)
                   .convert_to<double>()));

  A  = 2.00e30;
  Ea = 2.00e5;

  Arr.setA(A);
  Arr.setEa(Ea);

  Omega = Arr(N, t.data(), T.data());

  CHECK(Omega.convert_to<double>() ==
        Approx((A * exp(-Ea / (MKS::R * 410)) * tau +
                A * exp(-Ea / (MKS::R * 310)) * 3 * tau)
                   .convert_to<double>())
            .epsilon(0.001));

  A  = 2.00e30;
  Ea = 2.00e10;

  Arr.setA(A);
  Arr.setEa(Ea);

  Omega = Arr(N, t.data(), T.data());

  CHECK(Omega > 0);

  // this will result in a number too large for double
  A  = 2.00e300;
  Ea = -2.00e5;

  Arr.setA(A);
  Arr.setEa(Ea);

  Omega = Arr(N, t.data(), T.data());

  CHECK(Omega != std::numeric_limits<cpp_dec_float_100>::infinity());
}
