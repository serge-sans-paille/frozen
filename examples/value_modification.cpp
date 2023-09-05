#include <frozen/set.h>
#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <iostream>

/// MAYBE_CONSTINIT expands to `constinit` if available.
#if __cpp_constinit
#define MAYBE_CONSTINIT constinit
#else
#define MAYBE_CONSTINIT
#endif

// To make a frozen::unordered_map where you can modify the values, make a
// non-constexpr instance. If available, prefer to use constinit. It will
// initialize the map during compilation and detect any exceptions.
MAYBE_CONSTINIT static frozen::unordered_map<frozen::string, int, 2> fruits = {
    {"n_apples", 0},
    {"n_pears", 0},
};

// Force conversion to a const reference. std::cref(ref).get() fails with libc++
template <typename T>
constexpr decltype(auto) const_ref(const T& ref) noexcept {
  return ref;
}

int main() {
  // Update the values using at()
  fruits.at("n_apples") = 10;
  fruits.at("n_pears") = fruits.at("n_apples") * 2;
  std::cout << "n_apples: " << fruits.at("n_apples") << std::endl;
  std::cout << "n_pears: " << fruits.at("n_pears") << std::endl;

  static_assert(std::is_same<decltype(const_ref(fruits).at("n_apples")), const int&>::value, "");
  static_assert(std::is_same<decltype(fruits.at("n_apples")), int&>::value, "");

  // You can also update values via the iterator returned by find()
  auto found = fruits.find("n_apples");
  found->second = 0;
  std::cout << "n_apples: " << fruits.at("n_apples") << std::endl;

  // Range also works
  auto range = fruits.equal_range("n_apples");
  range.first->second = 1337;
  std::cout << "n_apples: " << fruits.at("n_apples") << std::endl;
}
