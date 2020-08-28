/*
/home/ed/bin/bin/g++ -std=c++17 -g -Wall -Wextra -o test_midpoint test_midpoint.cpp
*/

#include <limits>
#include <type_traits>

#include "midpoint.h"

#include <iostream>

template<typename Int>
  bool
  test_twos_comp()
  {
    const std::make_unsigned_t<Int> a = 10, b = 5;
    const auto mab = std::midpoint(a, b);
    std::cout << "mab = " << int(mab) << '\n';

    return true;
  }

template<typename Real>
  bool
  test_huge()
  {
    const auto huge = std::numeric_limits<Real>::max();
    const auto a = -huge;
    const auto b = +huge;
    const auto c = std::midpoint(a, b);
    std::cout << "c = " << c << '\n';
    std::cout << "c == 0: " << std::boolalpha << (c == Real{0}) << '\n';
    return c == Real{0};
  }

template<typename Tp>
  bool
  test_ptr()
  {
    constexpr int num = 21;
    constexpr Tp arr[num]{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    const auto a = arr;
    const auto b = arr + num;
    auto ab = std::midpoint(a, b);
    std::cout << "a = " << a << "; b = " << b << "; ab = " << ab << '\n';
    std::cout << "ab - a = " << ab - a << "; ab - b = " << ab - b << '\n';
    std::cout << "ab = " << ab << "; *ab = " << *ab << '\n';
    auto ba = std::midpoint(b-1, a-1); // rbegin, rend
    std::cout << "ba - (b-1) = " << ba - (b-1) << "; ba - (a-1) = " << ba - (a-1) << '\n';
    std::cout << "ba = " << ba << "; *ba = " << *ba << '\n';
    return true;
  }

int
main()
{
  bool ok = true;

  const int a = 5, b = 10;
  const auto mab = std::midpoint(a, b);
  std::cout << "mab = " << mab << '\n';

  std::cout << "\ntest_huge\n";
  ok = ok && test_huge<float>();
  ok = ok && test_huge<double>();
  ok = ok && test_huge<long double>();

  std::cout << "\ntest_twos_comp\n";
  ok = ok && test_twos_comp<unsigned char>();
  ok = ok && test_twos_comp<short>();
  ok = ok && test_twos_comp<int>();
  ok = ok && test_twos_comp<long>();
  ok = ok && test_twos_comp<long long>();

  std::cout << "\ntest_ptr\n";
  ok = ok && test_ptr<int>();

  return ok;
}
