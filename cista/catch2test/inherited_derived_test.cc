#include <catch2/catch_test_macros.hpp>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/containers/hash_map.h"
#include "cista/containers/string.h"
#include "cista/containers/vector.h"
#include "cista/reflection/comparable.h"
#include "cista/serialization.h"
#endif

TEST_CASE("inherited derived", "[serialization][reflection]") {
  namespace data = cista::offset;
  constexpr auto MODE = cista::mode::WITH_VERSION;

  struct parent {
    parent() = default;
    explicit parent(int a) : x_{a}, y_{a} {}
    auto cista_members() { return std::tie(x_, y_); }
    int x_, y_;
  };

  struct child : parent {
    child() = default;
    explicit child(int a) : parent{a}, z_{a} {}
    auto cista_members() { return std::tie(*static_cast<parent*>(this), z_); }
    int z_;
  };

  using t = data::hash_map<child, int>;

  cista::byte_buf b;
  {  // Serialize.
    auto x = t{{child{1}, 2}, {child{3}, 4}};
    b = cista::serialize<MODE>(x);
  }

  // Deserialize.
  auto deserialized = cista::deserialize<t, MODE>(b);
  REQUIRE(deserialized != nullptr);
  CHECK(deserialized->size() == 2);
}