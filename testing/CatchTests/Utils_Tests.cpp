#include "catch.hpp"
#include "fakeit.hpp"

#include "Utils/LinearRegression.hpp"

TEST_CASE( "Linear Regression Function", "[utils]" ) {

  typedef double DataType;

  int N = 5;
  Matrix<DataType, Dynamic,1> x(N),y(N);

  // y = 2x + 1
  x << 1, 2, 3, 4, 5;
  y << 3, 5, 7, 9, 11;

  auto beta = libArrhenius::LinearRegression(x,y);
  CHECK( beta(0) == Approx(1) );
  CHECK( beta(1) == Approx(2) );


  // y = -2x + 10
  x << 1, 2, 3, 4, 5;
  y << 8, 6, 4, 2, 0;


  beta = libArrhenius::LinearRegression(x,y);
  CHECK( beta(0) == Approx(10) );
  CHECK( beta(1) == Approx(-2) );











}
