#include <frozen/set.h>
#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <iostream>


int main() {
  frozen::unordered_map<frozen::string, int, 2> fruits = {
      {"n_apples", 0},
      {"n_pears", 0},
  };

  std::cout << fruits.at("n_apples") << std::endl;
  std::cout << fruits.at("n_pears") << std::endl;

  fruits.at("n_apples") = 10;
  fruits.at("n_pears") = fruits.at("n_apples") * 2;

  std::cout << fruits.at("n_apples") << std::endl;
  std::cout << fruits.at("n_pears") << std::endl;

  auto found = fruits.find("n_oranges");
  if (found == fruits.end()) {
    std::cout << "no oranges, as expected" << std::endl;
  }

  auto range = fruits.equal_range("n_oranges");
}
