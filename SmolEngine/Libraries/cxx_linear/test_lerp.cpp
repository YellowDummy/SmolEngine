/*
/home/ed/bin/bin/g++ -std=c++17 -g -Wall -Wextra -o test_lerp test_lerp.cpp
*/

#include <limits>
#include <type_traits>

#include "lerp.h"

#include <iostream>

template<typename Real>
  bool
  test()
  {
    const auto a = Real{5};
    const auto b = Real{10};
    const auto t = Real{0.75L};
    const auto x = std::lerp(a, b, t);

    std::cout << "x = " << x << '\n';

    return true;
  }

template<typename Real>
  bool
  test_ends()
  {
    const auto a = Real{-3};
    const auto b = Real{+5};

    const auto ca = std::lerp(a, b, Real{0});
    std::cout << "ca = " << ca << '\n';
    std::cout << "ca == a: " << std::boolalpha << (ca == a) << '\n';

    const auto cb = std::lerp(a, b, Real{1});
    std::cout << "cb = " << cb << '\n';
    std::cout << "cb == b: " << std::boolalpha << (cb == b) << '\n';

    return ca == a && cb == b;
  }

template<typename Real>
  bool
  test_nan()
  {
    const auto a = Real{5};
    const auto b = Real{10};
    const auto t = Real{0.75L};

    const auto anan = std::numeric_limits<Real>::quiet_NaN();
    const auto bnan = std::numeric_limits<Real>::quiet_NaN();
    const auto tnan = std::numeric_limits<Real>::quiet_NaN();

    const auto nan = std::isnan(std::lerp(anan, b, t))
		  && std::isnan(std::lerp(a, bnan, t))
		  && std::isnan(std::lerp(a, b, tnan));

    std::cout << "nan: " << std::boolalpha << nan << '\n';

    return nan;
  }

template<typename Real>
  bool
  test_subnorm()
  {
    const auto denorm = std::numeric_limits<Real>::denorm_min();

    const auto a = Real{5} * denorm;
    const auto b = Real{10} * denorm;
    const auto t = Real{0.75L};
    const auto x = std::lerp(a, b, t);
    std::cout << "x: " << x / denorm << '\n';

    const auto a00 = Real{500} * denorm;
    const auto b00 = Real{1000} * denorm;
    const auto x00 = std::lerp(a00, b00, t);
    std::cout << "x00: " << x00 / denorm << '\n';
    return true;
  }

template<typename Real>
  bool
  test_inf()
  {
    const auto a = Real{5};
    const auto b = a;
    const auto t = std::numeric_limits<Real>::infinity();
    const auto c = std::lerp(a, b, t);
    std::cout << "c = " << c << '\n';
    std::cout << "c == a: " << std::boolalpha << (c == a) << '\n';
    return c == a;
  }

template<typename Real>
  bool
  test_huge()
  {
    const auto max = std::numeric_limits<Real>::max() / 16;
    const auto a = Real{5} * max;
    const auto b = Real{10} * max;
    const auto t = Real{3} / Real{4};
    const auto c = std::lerp(a, b, t);
    std::cout << "c = " << c / max << '\n';
    std::cout << "c == b/2: " << std::boolalpha << (c == Real{8.75} * max) << '\n';
    return c == b / Real{2};
  }

int
main()
{
  bool ok = true;

  std::cout << "\ntest\n";
  ok = ok && test<float>();
  ok = ok && test<double>();
  ok = ok && test<long double>();

  std::cout << "\ntest_nan\n";
  ok = ok && test_nan<float>();
  ok = ok && test_nan<double>();
  ok = ok && test_nan<long double>();

  std::cout << "\ntest_ends\n";
  ok = ok && test_ends<float>();
  ok = ok && test_ends<double>();
  ok = ok && test_ends<long double>();

  std::cout << "\ntest_inf\n";
  ok = ok && test_inf<float>();
  ok = ok && test_inf<double>();
  ok = ok && test_inf<long double>();

  std::cout << "\ntest_subnorm\n";
  ok = ok && test_subnorm<float>();

  std::cout << "\ntest_huge\n";
  ok = ok && test_huge<float>();
  ok = ok && test_huge<double>();
  ok = ok && test_huge<long double>();

  return ok;
}
