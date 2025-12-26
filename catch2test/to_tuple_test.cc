#include <string>

#include <catch2/catch_test_macros.hpp>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/reflection/to_tuple.h"
#endif

struct a {
  int i_ = 1;
  int j_ = 2;
  double d_ = 100.0;
  std::string s_ = "hello world";
};

TEST_CASE("to_tuple") {
  a instance;

  REQUIRE(instance.i_ == 1);
  REQUIRE(instance.j_ == 2);
  REQUIRE(instance.d_ == 100.0);
  REQUIRE(instance.s_ == "hello world");

  std::get<0>(cista::to_tuple(instance)) = 5;
  std::get<1>(cista::to_tuple(instance)) = 7;
  std::get<2>(cista::to_tuple(instance)) = 2.0;
  std::get<3>(cista::to_tuple(instance)) = "yeah";

  REQUIRE(instance.i_ == 5);
  REQUIRE(instance.j_ == 7);
  REQUIRE(instance.d_ == 2.0);
  REQUIRE(instance.s_ == "yeah");
}