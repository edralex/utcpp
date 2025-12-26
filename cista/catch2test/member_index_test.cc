#include <catch2/catch_test_macros.hpp>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/reflection/member_index.h"
#endif

TEST_CASE("member_index", "[reflection]") {
  struct test {
    int a, b, c;
  };
  CHECK(0 == cista::member_index(&test::a));
  CHECK(1 == cista::member_index(&test::b));
  CHECK(2 == cista::member_index(&test::c));
}