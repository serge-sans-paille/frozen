#include <frozen/set.h>

constexpr frozen::set<int, 1> value = {1};

static_assert(value.count(1) == 1, "");
