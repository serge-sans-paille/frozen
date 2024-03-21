#ifdef EXAMPLES_USE_MODULE
import frozen;
#else 
#include <frozen/set.h>
#endif

static constexpr frozen::set<unsigned, 3> supported_sizes = {
  1, 2, 4
};

static_assert(supported_sizes.count(sizeof(short)), "unsupported size");

int main() { return 0; }
