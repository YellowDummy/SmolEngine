#ifndef COMPLEX_MIDPOINT_H
#define COMPLEX_MIDPOINT_H 1

#include <complex>
#include "midpoint.h"

namespace std
{

  template<typename _Float>
    constexpr std::enable_if_t<std::is_floating_point_v<_Float>,
			       std::complex<_Float>>
    midpoint(const std::complex<_Float>& __a,
	     const std::complex<_Float>& __b) noexcept
    {
      using __cmplx = std::complex<_Float>;
      return __cmplx(std::midpoint(std::real(__a), std::real(__b)),
		     std::midpoint(std::imag(__a), std::imag(__b)));
    }

} // namespace std

#endif // COMPLEX_MIDPOINT_H
