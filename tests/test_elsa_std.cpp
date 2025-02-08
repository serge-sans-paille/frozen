// include catch before import std to avoid STL issues
#include "catch.hpp"

#ifdef FROZEN_USE_STD_MODULE
import std;
#endif 

#ifdef FROZEN_USE_MODULE
import frozen;
#else 
#include <frozen/bits/defines.h>
#endif

#ifdef FROZEN_LETITGO_HAS_STRING_VIEW
#ifndef FROZEN_USE_MODULE
#include <frozen/bits/elsa_std.h>
#include <frozen/map.h>
#include <frozen/set.h>
#include <frozen/unordered_map.h>
#include <frozen/unordered_set.h>
#endif

#ifndef FROZEN_USE_STD_MODULE
#include <string_view>
#include <tuple>
#endif

#ifdef FROZEN_LETITGO_HAS_CONSTEXPR_STRING

#ifndef FROZEN_USE_STD_MODULE
#include <string>
#endif

using StringTypes = std::tuple<std::string_view, std::string>;

#else

using StringTypes = std::tuple<std::string_view>;

#endif

TEMPLATE_LIST_TEST_CASE("frozen containers work with standard string types", "[elsa_std]", StringTypes)
{
    constexpr TestType str = "string";

    const auto is_found_in = [&str](const auto& container){ return container.count(str) == 1; };

    constexpr frozen::set<TestType, 1> set{str};
    constexpr frozen::unordered_set<TestType, 1> unordered_set{str};
    constexpr auto map = frozen::make_map<TestType, int>({{str, 0}});
    constexpr auto unordered_map = frozen::make_unordered_map<TestType, int>({{str, 0}});

    REQUIRE(is_found_in(set));
    REQUIRE(is_found_in(unordered_set));
    REQUIRE(is_found_in(map));
    REQUIRE(is_found_in(unordered_map));
}

#endif // FROZEN_LETITGO_HAS_STRING_VIEW
