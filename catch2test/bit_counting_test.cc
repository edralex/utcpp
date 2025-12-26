#include <catch2/catch_test_macros.hpp>
#include <limits>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/bit_counting.h"
#endif

TEST_CASE("count 64bit zero", "[bit_counting]") {
  uint64_t t = 0;
  CHECK(cista::trailing_zeros(t) == 64);
  CHECK(cista::leading_zeros(t) == 64);
}

TEST_CASE("count 64bit one", "[bit_counting]") {
  uint64_t t = 1ULL;
  CHECK(cista::trailing_zeros(t) == 0);
  CHECK(cista::leading_zeros(t) == 63);
}

TEST_CASE("count 64bit higher", "[bit_counting]") {
  uint64_t t = static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()) + 1;
  CHECK(cista::trailing_zeros(t) == 32);
  CHECK(cista::leading_zeros(t) == 31);
}

TEST_CASE("count 64bit any", "[bit_counting]") {
  uint64_t t = 7ULL << 30ULL;
  CHECK(cista::trailing_zeros(t) == 30);
  CHECK(cista::leading_zeros(t) == 31);
}

TEST_CASE("count 32bit zero", "[bit_counting]") {
  uint32_t t = 0;
  CHECK(cista::trailing_zeros(t) == 32);
  CHECK(cista::leading_zeros(t) == 32);
}

TEST_CASE("count 32bit one", "[bit_counting]") {
  uint32_t t = 1U;
  CHECK(cista::trailing_zeros(t) == 0);
  CHECK(cista::leading_zeros(t) == 31);
}

TEST_CASE("count 32bit highest", "[bit_counting]") {
  uint32_t t = 1U << 31U;
  CHECK(cista::trailing_zeros(t) == 31);
  CHECK(cista::leading_zeros(t) == 0);
}

TEST_CASE("count 32bit any", "[bit_counting]") {
  uint32_t t = 7U << 30U;
  CHECK(cista::trailing_zeros(t) == 30);
  CHECK(cista::leading_zeros(t) == 0);
}