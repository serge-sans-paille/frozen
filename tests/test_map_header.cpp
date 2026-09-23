#include <frozen/map.h>

constexpr frozen::map<int, int, 1> value = {
    {1, 1},
};

static_assert(value.at(1) == 1, "");
