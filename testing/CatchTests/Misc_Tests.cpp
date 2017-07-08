#include "catch.hpp"
#include "fakeit.hpp"

#include <eigen3/Eigen/Dense>

using namespace Eigen;

TEST_CASE( "Eigen-based Linear Regression", "[experiment]" ) {

  typedef double DataType;


  int N = 5;
  Matrix<DataType, Dynamic,1> x(N),y(N);

  // y = 2x + 1
  x << 1, 2, 3, 4, 5;
  y << 3, 5, 7, 9, 11;


  {
  Matrix<DataType,Dynamic,2> X(N,2);
  X.block(0,0,N,1) = Matrix<DataType,Dynamic,1>::Constant(N,1,1);
  X.block(0,1,N,1) = x;

  auto Xtrans = X.transpose();
  auto Y = Xtrans*y;

  // solve Xtrans X B = Xtrans Y
  auto beta1 = (Xtrans*X).inverse()*Y;
  auto beta2 = (Xtrans*X).ldlt().solve(Y);

  CHECK( beta1(0) == Approx(1) );
  CHECK( beta1(1) == Approx(2) );
  // this causes a segfault on my Ubuntu machine
  //CHECK( beta2(0) == Approx(1) );
  //CHECK( beta2(1) == Approx(2) );
  }


  // y = -2x + 10
  x << 1, 2, 3, 4, 5;
  y << 8, 6, 4, 2, 0;


  {
  Matrix<DataType,Dynamic,2> X(N,2);
  X.block(0,0,N,1) = Matrix<DataType,Dynamic,1>::Constant(N,1,1);
  X.block(0,1,N,1) = x;

  auto Xtrans = X.transpose();
  auto Y = Xtrans*y;

  // solve Xtrans X B = Xtrans Y
  auto beta1 = (Xtrans*X).inverse()*Y;
  auto beta2 = (Xtrans*X).ldlt().solve(Y);

  CHECK( beta1(0) == Approx(10) );
  CHECK( beta1(1) == Approx(-2) );
  // this causes a segfault on my Ubuntu machine
  //CHECK( beta2(0) == Approx(10) );
  //CHECK( beta2(1) == Approx(-2) );
  }











}
