#include <frozen/set.h>
#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <iostream>


int main() {
  // To make a frozen::unordered_map where you can modify the values, make a
  // non-constexpr instance:
  frozen::unordered_map<frozen::string, int, 2> fruits = {
      {"n_apples", 0},
      {"n_pears", 0},
  };

  // You can update the values:
  fruits.at("n_apples") = 10;
  fruits.at("n_pears") = fruits.at("n_apples") * 2;
  std::cout << fruits.at("n_apples") << std::endl;
  std::cout << fruits.at("n_pears") << std::endl;

  // Find also works
  auto found = fruits.find("n_oranges");
  if (found == fruits.end()) {
    std::cout << "no oranges, as expected" << std::endl;
  }

  // Range also works
  auto range = fruits.equal_range("n_apples");
}
