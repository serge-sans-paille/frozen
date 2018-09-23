#include <utility>
#include <frozen/map.h>

/* rough approximation */
constexpr uint64_t isqrt(uint64_t v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v |= v >> 32;
  v++;
  return v >> 1;
}

struct prime_sieve {
constexpr bool operator()(uint64_t v) {
  if(v%2 == 0 && v != 2) return false;

  for(uint64_t i = 3, n = isqrt(v); i < n; i +=2)
    if(v % i == 0) return false;

  return true;
}
};

template<class F, class T, T... Vs>
struct memoized {

    auto operator()(T v) const
    {
        static constexpr frozen::map<T, decltype(F{}(v)), sizeof...(Vs)> cached = {{Vs, F{}(Vs)}...};
        auto where = cached.find(v);
        if(where != cached.end()) {
            return where->second;
        }
        return F{}(v);
    }
};

#include <iostream>
int main(int argc, char** argv) {
  memoized<prime_sieve, uint64_t, 0,1,2,3,4,5,6,7,8,9> ps;
  std::cout << ps(7) << " " << ps(100) << "\n";
  return 0;
}
