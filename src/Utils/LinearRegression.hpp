
#ifndef Utils_LinearRegression_hpp
#define Utils_LinearRegression_hpp

/** @file LinearRegression.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 07/07/17
  */

#include <eigen3/Eigen/Dense>
using namespace Eigen;

namespace libArrhenius {

template<typename T>
Matrix<T,2,1> LinearRegression( const Matrix<T,Dynamic,1> &x, const Matrix<T,Dynamic,1> &y )
{
  // given vectors x = | x0 | and y = | y0 | of x,y pairs,
  //                   | x1 | and     | y1 |
  //                   | .. | and     | .. |
  //                   | xn | and     | yn |
  //
  // construct matrix X = | 1 x0 ] see https://en.wikipedia.org/wiki/Linear_least_squares_(mathematics)
  //                      | 1 x1 ]
  //                      | .... ]
  //                      | 1 xn ]
  //
  // then coefficients matrix B = | b | that minimize sum of residuals is
  //                              | m |
  //
  // is found by solveing X^T X B = X^T y,
  // so
  //       B = (X^T X)^-1 X^T y;

  // matrix X
  assert( x.size() == y.size() );

  auto N = x.size();

  Matrix<T,Dynamic,2> X(N,2);
  // fill first column with 1's
  X.block(0,0,N,1) = Matrix<T,Dynamic,1>::Constant(N,1,1);
  // fill second column with x values
  X.block(0,1,N,1) = x;

  // need the transpose of X
  auto Xtrans = X.transpose();

  return (Xtrans*X).inverse()*Xtrans*y;
}

}


#endif // include protector
