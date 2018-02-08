#include <benchmark/benchmark.h>

#include <frozen/set.h>
#include <frozen/string.h>

#include <set>
#include <string>

static constexpr frozen::set<frozen::string, 32> Keywords{
    "auto",     "break",  "case",    "char",   "const",    "continue",
    "default",  "do",     "double",  "else",   "enum",     "extern",
    "float",    "for",    "goto",    "if",     "int",      "long",
    "register", "return", "short",   "signed", "sizeof",   "static",
    "struct",   "switch", "typedef", "union",  "unsigned", "void",
    "volatile", "while"};

static auto const* volatile Some = &Keywords;

static void BM_StrInFzSet(benchmark::State& state) {
  for (auto _ : state) {
    for(auto kw : *Some)
    volatile bool status = Keywords.count(kw);
  }
}
BENCHMARK(BM_StrInFzSet);

static const std::set<frozen::string> Keywords_(Keywords.begin(), Keywords.end());

static void BM_StrInStdSet(benchmark::State& state) {
  for (auto _ : state) {
    for(auto kw : *Some)
    volatile bool status = Keywords_.count(kw);
  }
}

BENCHMARK(BM_StrInStdSet);



static const frozen::string SomeStrings[32] = {
    "auto0",     "break0",  "case0",    "char0",   "const0",    "continue0",
    "default0",  "do0",     "double0",  "else0",   "enum0",     "extern0",
    "float0",    "for0",    "goto0",    "if0",     "int0",      "long0",
    "register0", "return0", "short0",   "signed0", "sizeof0",   "static0",
    "struct0",   "switch0", "typedef0", "union0",  "unsigned0", "void0",
    "volatile0", "while0"};
static auto const * volatile SomeStringsPtr = &SomeStrings;

static void BM_StrNotInFzSet(benchmark::State& state) {
  for (auto _ : state) {
    for(auto kw : *SomeStringsPtr)
    volatile bool status = Keywords.count(kw);
  }
}
BENCHMARK(BM_StrNotInFzSet);

static void BM_StrNotInStdSet(benchmark::State& state) {
  for (auto _ : state) {
    for(auto kw : *SomeStringsPtr)
    volatile bool status = Keywords_.count(kw);
  }
}
BENCHMARK(BM_StrNotInStdSet);
