#include <benchmark/benchmark.h>

#include <frozen/unordered_map.h>
#include <frozen/string.h>

#include <algorithm>
#include <array>
#include <string>
#include <unordered_map>

static constexpr frozen::unordered_map<frozen::string, frozen::string, 32> Keywords{
    {"auto", "keyword"}, {"break", "keyword"}, {"case", "keyword"}, {"char", "keyword"}, {"const", "keyword"}, {"continue", "keyword"},
    {"default", "keyword"}, {"do", "keyword"}, {"double", "keyword"}, {"else", "keyword"}, {"enum", "keyword"}, {"extern", "keyword"},
    {"float", "keyword"}, {"for", "keyword"}, {"goto", "keyword"}, {"if", "keyword"}, {"int", "keyword"}, {"long", "keyword"},
    {"register", "keyword"}, {"return", "keyword"}, {"short", "keyword"}, {"signed", "keyword"}, {"sizeof", "keyword"}, {"static", "keyword"},
    {"struct", "keyword"}, {"switch", "keyword"}, {"typedef", "keyword"}, {"union", "keyword"}, {"unsigned", "keyword"},
    {"void", "keyword"}, {"volatile", "keyword"}, {"while", "keyword"}
};

static auto const *volatile Some = &Keywords;

static void BM_StrInFzUnorderedMap(benchmark::State &state)
{
  for (auto _ : state)
  {
    for (auto kw : *Some)
    {
      volatile bool status = Keywords.count(kw.first);
      benchmark::DoNotOptimize(status);
    }
  }
}
BENCHMARK(BM_StrInFzUnorderedMap);

static const std::unordered_map<frozen::string, frozen::string> Keywords_(Keywords.begin(), Keywords.end());

static void BM_StrInStdUnorderedMap(benchmark::State &state)
{
  for (auto _ : state)
  {
    for (auto kw : *Some)
    {
      volatile bool status = Keywords_.count(kw.first);
      benchmark::DoNotOptimize(status);
    }
  }
}

BENCHMARK(BM_StrInStdUnorderedMap);

static const frozen::string SomeStrings[32] = {
    "auto0", "break0", "case0", "char0", "const0", "continue0",
    "default0", "do0", "double0", "else0", "enum0", "extern0",
    "float0", "for0", "goto0", "if0", "int0", "long0",
    "register0", "return0", "short0", "signed0", "sizeof0", "static0",
    "struct0", "switch0", "typedef0", "union0", "unsigned0", "void0",
    "volatile0", "while0"};
static auto const *volatile SomeStringsPtr = &SomeStrings;

static void BM_StrNotInFzUnorderedMap(benchmark::State &state)
{
  for (auto _ : state)
  {
    for (auto kw : *SomeStringsPtr)
    {
      volatile bool status = Keywords.count(kw);
      benchmark::DoNotOptimize(status);
    }
  }
}
BENCHMARK(BM_StrNotInFzUnorderedMap);

static void BM_StrNotInStdUnorderedMap(benchmark::State &state)
{
  for (auto _ : state)
  {
    for (auto kw : *SomeStringsPtr)
    {
      volatile bool status = Keywords_.count(kw);
      benchmark::DoNotOptimize(status);
    }
  }
}
BENCHMARK(BM_StrNotInStdUnorderedMap);
