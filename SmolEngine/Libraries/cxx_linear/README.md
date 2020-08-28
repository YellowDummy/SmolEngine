

# C++ midpoint and lerp functions.

This gives us [well-behaved interpolation for numbers and pointers](https://wg21.link/p0811)

These just got voted into C++20!

  float a = 3.2f;
  float b = 7.8f;
  float t = 0.75;
  auto x = std::lerp(a, b, t);

