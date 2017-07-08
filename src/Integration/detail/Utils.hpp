#ifndef Integration_detail_Utils_hpp
#define Integration_detail_Utils_hpp

/** @file Utils.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 07/08/17
  */

namespace libArrhenius {
namespace Integration {
namespace detail {
// we need an add function that can be used to reduce user-defined types with
// openmp
template<typename Real>
Real add(Real r, Real n)
{
  return r+n;
}

}
}
}




#endif // include protector
