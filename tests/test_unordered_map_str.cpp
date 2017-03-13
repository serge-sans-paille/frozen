#include <chrono>
#include <frozen/string.h>
#include <frozen/unordered_map.h>
#include <iostream>
#include <unordered_map>

#include "catch.hpp"
using namespace frozen::string_literals;

TEST_CASE("frozen::unordered_map<str, int> <> std::unordered_map",
          "[unordered_map]") {
#define INIT_SEQ                                                               \
  {"19"_s, 19}, {"1"_s, 1}, {"2"_s, 2}, {"4"_s, 4}, {"5"_s, 5}, {"6"_s, 6},    \
      {"7"_s, 7}, {"8"_s, 8}, {"9"_s, 9}, {"10"_s, 10}, {"11"_s, 11},          \
      {"111"_s, 111}, {"1112"_s, 1112}, {"1115"_s, 1115}, {"1118"_s, 1118},    \
      {"1110"_s, 1110}, {"1977"_s, 1977}, {"177"_s, 177}, {"277"_s, 277},      \
      {"477"_s, 477}, {"577"_s, 577}, {"677"_s, 677}, {"777"_s, 777},          \
      {"877"_s, 877}, {"977"_s, 977}, {"1077"_s, 1077}, {"1177"_s, 1177},      \
      {"11177"_s, 11177}, {"111277"_s, 111277}, {"111577"_s, 111577},          \
      {"111877"_s, 111877}, {"111077"_s, 111077}, {"1999"_s, 1999},            \
      {"199"_s, 199}, {"299"_s, 299}, {"499"_s, 499}, {"599"_s, 599},          \
      {"699"_s, 699}, {"799"_s, 799}, {"899"_s, 899}, {"999"_s, 999},          \
      {"1099"_s, 1099}, {"1199"_s, 1199}, {"11199"_s, 11199},                  \
      {"111299"_s, 111299}, {"111599"_s, 111599}, {"111899"_s, 111899},        \
      {"111099"_s, 111099}, {"197799"_s, 197799}, {"17799"_s, 17799},          \
      {"27799"_s, 27799}, {"47799"_s, 47799}, {"57799"_s, 57799},              \
      {"67799"_s, 67799}, {"77799"_s, 77799}, {"87799"_s, 87799},              \
      {"97799"_s, 97799}, {"107799"_s, 107799}, {"117799"_s, 117799},          \
      {"1117799"_s, 1117799}, {"11127799"_s, 11127799},                        \
      {"11157799"_s, 11157799}, {"11187799"_s, 11187799},                      \
      {"11107799"_s, 11107799}, {"1988"_s, 1988}, {"188"_s, 188},              \
      {"288"_s, 288}, {"488"_s, 488}, {"588"_s, 588}, {"688"_s, 688},          \
      {"788"_s, 788}, {"888"_s, 888}, {"988"_s, 988}, {"1088"_s, 1088},        \
      {"1188"_s, 1188}, {"11188"_s, 11188}, {"111288"_s, 111288},              \
      {"111588"_s, 111588}, {"111888"_s, 111888}, {"111088"_s, 111088},        \
      {"197788"_s, 197788}, {"17788"_s, 17788}, {"27788"_s, 27788},            \
      {"47788"_s, 47788}, {"57788"_s, 57788}, {"67788"_s, 67788},              \
      {"77788"_s, 77788}, {"87788"_s, 87788}, {"97788"_s, 97788},              \
      {"107788"_s, 107788}, {"117788"_s, 117788}, {"1117788"_s, 1117788},      \
      {"11127788"_s, 11127788}, {"11157788"_s, 11157788},                      \
      {"11187788"_s, 11187788}, {"11107788"_s, 11107788},                      \
      {"199988"_s, 199988}, {"19988"_s, 19988}, {"29988"_s, 29988},            \
      {"49988"_s, 49988}, {"59988"_s, 59988}, {"69988"_s, 69988},              \
      {"79988"_s, 79988}, {"89988"_s, 89988}, {"99988"_s, 99988},              \
      {"109988"_s, 109988}, {"119988"_s, 119988}, {"1119988"_s, 1119988},      \
      {"11129988"_s, 11129988}, {"11159988"_s, 11159988},                      \
      {"11189988"_s, 11189988}, {"11109988"_s, 11109988},                      \
      {"19779988"_s, 19779988}, {"1779988"_s, 1779988},                        \
      {"2779988"_s, 2779988}, {"4779988"_s, 4779988}, {"5779988"_s, 5779988},  \
      {"6779988"_s, 6779988}, {"7779988"_s, 7779988}, {"8779988"_s, 8779988},  \
      {"9779988"_s, 9779988}, {"10779988"_s, 10779988},                        \
      {"11779988"_s, 11779988}, {"111779988"_s, 111779988},                    \
      {"1112779988"_s, 1112779988}, {"1115779988"_s, 1115779988},              \
      {"1118779988"_s, 1118779988}, {                                          \
    "1110779988"_s, 1110779988                                                 \
  }

  const std::unordered_map<frozen::string, int> std_map = {INIT_SEQ};
  constexpr frozen::unordered_map<frozen::string, int, 128> frozen_map = {
      INIT_SEQ};
  REQUIRE(std_map.size() == frozen_map.size());

  SECTION("checking size and content") {
    REQUIRE(std_map.size() == frozen_map.size());
    for (auto v : std_map)
      REQUIRE(frozen_map.count(v.first));

    for (auto v : frozen_map)
      REQUIRE(std_map.count(v.first));
  }

  SECTION("checking minimal performance requirements") {
    std::initializer_list<std::pair<frozen::string, int>> data = {INIT_SEQ};
    auto std_start = std::chrono::steady_clock::now();
    for (int i = 0; i < 10000; ++i)
      for (auto val : data)
        volatile const auto __attribute__((unused)) c =
            std_map.count(val.first);
    auto std_stop = std::chrono::steady_clock::now();
    auto std_diff = std_stop - std_start;
    auto std_duration =
        std::chrono::duration<double, std::milli>(std_diff).count();
    std::cout << "std::unordered_map<str, int>: " << std_duration << " ms"
              << std::endl;

    auto frozen_start = std::chrono::steady_clock::now();
    for (int i = 0; i < 10000; ++i)
      for (auto val : data)
        volatile const auto __attribute__((unused)) c =
            frozen_map.count(val.first);
    auto frozen_stop = std::chrono::steady_clock::now();
    auto frozen_diff = frozen_stop - frozen_start;
    auto frozen_duration =
        std::chrono::duration<double, std::milli>(frozen_diff).count();
    std::cout << "frozen::unordered_map<str, int>: " << frozen_duration << " ms"
              << std::endl;

    REQUIRE(std_duration > frozen_duration);
  }
}
