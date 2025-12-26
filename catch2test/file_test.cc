#include <fstream>
#include <iostream>
#include <catch2/catch_test_macros.hpp>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/reflection/for_each_field.h"
#include "cista/targets/file.h"
#endif

using namespace cista;

TEST_CASE("file", "[file]") {
  struct test {
    int a_ = 1;
    int b_ = 2;
    int c_ = 3;
  } t;

  struct test1 {
    uint64_t number_ = 77;
  } t1;

  auto start = cista::offset_t{};
  {
    file f{"test.bin", "w+"};
    start = f.write(&t, sizeof(t), std::alignment_of_v<test>);
    int i = 11;
    for_each_field(t, [start, &i, &t, &f](auto&& m) mutable {
      f.write(
          static_cast<size_t>(start + static_cast<offset_t>(
                                          reinterpret_cast<std::intptr_t>(&m) -
                                          reinterpret_cast<std::intptr_t>(&t))),
          i++);
    });

    start = f.write(&t1, sizeof(t1), std::alignment_of_v<test1>);
  }

  int i;
  std::ifstream f{"test.bin", std::ios_base::binary};
  for (int j = 0; j < 3; ++j) {
    f.read(reinterpret_cast<char*>(&i), sizeof(i));
    CHECK(i == 11 + j);
  }

  uint64_t number;
  f.seekg(start);
  f.read(reinterpret_cast<char*>(&number), sizeof(number));
  CHECK(number == 77);
}