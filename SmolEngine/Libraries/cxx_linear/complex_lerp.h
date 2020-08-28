#ifndef COMPLEX_LERP_H
#define COMPLEX_LERP_H 1

#include <complex>
#include "lerp.h"

namespace std
{

  template<typename _Float>
    constexpr std::enable_if_t<std::is_floating_point_v<_Float>,
			       std::complex<_Float>>
    lerp(const std::complex<_Float>& __a,
	 const std::complex<_Float>& __b, _Float __t)
    {
      using __cmplx = std::complex<_Float>;
      return __cmplx(std::lerp(std::real(__a), std::real(__b), __t),
		     std::lerp(std::imag(__a), std::imag(__b), __t)));
    }

} // namespace std

#endif // COMPLEX_LERP_H
