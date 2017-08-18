
#ifndef Utils_LinearRegression_hpp
#define Utils_LinearRegression_hpp

/** @file LinearRegression.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 07/07/17
  */

#include <Eigen/Dense>
using namespace Eigen;

namespace RUC {

/*
 * @breif Perform least-squared linear regression on a set of x-y data points.
 *
 * @param x vector containing x coordinates of point to fit.
 * @param y vector containing y coordinates of point to fit.
 * @return A 2x1 matrix containing the y-intercept (element 0) and slope (element 1) of the fit.
 */
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

  auto fit = (Xtrans*X).inverse()*Xtrans*y;

  Matrix<T,2,1> ret;
  ret[0] = fit[0];
  ret[1] = fit[1];

  return ret;
}

}


#endif // include protector
