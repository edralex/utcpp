#include <gtest/gtest.h>
#include <array>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/containers/variant.h"
#include "cista/mmap.h"
#include "cista/serialization.h"
#include "cista/targets/file.h"
#endif

namespace data = cista::offset;

namespace variant_test {
enum entry_idx_t {
  A_DEFAULT_CTOR,
  A_COPY_CTOR,
  A_MOVE_CTOR,
  A_MOVE_ASSIGN,
  A_COPY_ASSIGN,
  A_DTOR,
  B_DEFAULT_CTOR,
  B_COPY_CTOR,
  B_MOVE_CTOR,
  B_MOVE_ASSIGN,
  B_COPY_ASSIGN,
  B_DTOR,
  NUM_ENTRIES
};
static std::array<int, NUM_ENTRIES> entries;

struct a {
  a() { ++entries[A_DEFAULT_CTOR]; }
  a(a const&) { ++entries[A_COPY_CTOR]; }
  a(a&&) { ++entries[A_MOVE_CTOR]; }
  a& operator=(a&&) {
    ++entries[A_MOVE_ASSIGN];
    return *this;
  }
  a& operator=(a const&) {
    ++entries[A_COPY_ASSIGN];
    return *this;
  }
  ~a() { ++entries[A_DTOR]; }
};

struct b {
  b() { ++entries[B_DEFAULT_CTOR]; }
  b(b const&) { ++entries[B_COPY_CTOR]; }
  b(b&&) { ++entries[B_MOVE_CTOR]; }
  b& operator=(b&&) {
    ++entries[B_MOVE_ASSIGN];
    return *this;
  }
  b& operator=(b const&) {
    ++entries[B_COPY_ASSIGN];
    return *this;
  }
  ~b() { ++entries[B_DTOR]; }
};

}  // namespace variant_test

using namespace variant_test;

TEST(VariantTest, BasicMethods) {
  data::variant<a, b> v{a{}}, u{b{}};

  EXPECT_EQ(entries, (std::array<int, NUM_ENTRIES>{1, 0, 1, 0, 0, 1,  //
                                                   1, 0, 1, 0, 0, 1}));
  entries = {};

  v.apply(
      [](auto&& e) { EXPECT_TRUE((std::is_same_v<std::decay_t<decltype(e)>, a>)); });
  EXPECT_EQ(entries, (std::array<int, NUM_ENTRIES>{0, 0, 0, 0, 0, 0,  //
                                                   0, 0, 0, 0, 0, 0}));
  v = b{};
  EXPECT_EQ(entries, (std::array<int, NUM_ENTRIES>{0, 0, 0, 0, 0, 1,  //
                                                   1, 0, 1, 0, 0, 1}));
  entries = {};

  v.apply(
      [](auto&& e) { EXPECT_TRUE((std::is_same_v<std::decay_t<decltype(e)>, b>)); });
  EXPECT_EQ(entries, (std::array<int, NUM_ENTRIES>{0, 0, 0, 0, 0, 0,  //
                                                   0, 0, 0, 0, 0, 0}));

  v = a{};
  EXPECT_EQ(entries, (std::array<int, NUM_ENTRIES>{1, 0, 1, 0, 0, 1,  //
                                                   0, 0, 0, 0, 0, 1}));
  entries = {};

  v.apply(
      [](auto&& e) { EXPECT_TRUE((std::is_same_v<std::decay_t<decltype(e)>, a>)); });
  EXPECT_EQ(entries, (std::array<int, NUM_ENTRIES>{0, 0, 0, 0, 0, 0,  //
                                                   0, 0, 0, 0, 0, 0}));

  v = u;
  v.apply(
      [](auto&& e) { EXPECT_TRUE((std::is_same_v<std::decay_t<decltype(e)>, b>)); });
  EXPECT_EQ(entries, (std::array<int, NUM_ENTRIES>{0, 0, 0, 0, 0, 1,  //
                                                   0, 1, 0, 0, 0, 0}));
  entries = {};

  v = a{};
  u = b{};
  entries = {};

  v.swap(u);
  EXPECT_EQ(entries, (std::array<int, NUM_ENTRIES>{0, 0, 1, 0, 0, 1,  //
                                                   0, 0, 2, 0, 0, 2}));

  v = a{};
  u = a{};
  entries = {};
  v.swap(u);
  EXPECT_EQ(entries, (std::array<int, NUM_ENTRIES>{0, 0, 1, 2, 0, 1,  //
                                                   0, 0, 0, 0, 0, 0}));
}

TEST(VariantTest, Comparison) {
  data::variant<int, std::string> v{1}, u{std::string{"hello"}};
  static_assert(cista::variant_size_v<decltype(v)> == 2);

  EXPECT_EQ(v.index(), 0);
  EXPECT_EQ(u.index(), 1);
  EXPECT_TRUE(cista::holds_alternative<int>(v));
  EXPECT_TRUE(cista::holds_alternative<std::string>(u));

  EXPECT_TRUE(u > v);
  EXPECT_TRUE(v < u);
  EXPECT_TRUE(u >= v);
  EXPECT_TRUE(v <= u);
  EXPECT_TRUE(v != u);
  EXPECT_FALSE(v == u);

  u = 0;
  EXPECT_TRUE(u < v);
  EXPECT_TRUE(v > u);
  EXPECT_TRUE(v >= u);
  EXPECT_TRUE(u <= v);
  EXPECT_TRUE(v != u);
  EXPECT_FALSE(v == u);

  EXPECT_EQ(10, (v.emplace<int>(0) = 10));
  EXPECT_EQ(20, (v.emplace<0>(0) = 20));

  v = 0;
  EXPECT_FALSE(u < v);
  EXPECT_FALSE(v > u);
  EXPECT_TRUE(v >= u);
  EXPECT_TRUE(u <= v);
  EXPECT_TRUE(v == u);
  EXPECT_FALSE(v != u);
}

TEST(VariantTest, Get) {
  data::variant<int, std::string> v{1}, u{std::string{"hello"}};

  EXPECT_EQ(cista::get_if<0>(u), nullptr);
  EXPECT_EQ(cista::get_if<1>(v), nullptr);
  ASSERT_NE(cista::get_if<0>(v), nullptr);
  ASSERT_NE(cista::get_if<1>(u), nullptr);
  EXPECT_EQ(*cista::get_if<0>(v), 1);
  EXPECT_EQ(*cista::get_if<1>(u), std::string{"hello"});

  v.swap(u);

  EXPECT_EQ(std::get<int>(u), 1);
  EXPECT_EQ(std::get<std::string>(v), "hello");
  EXPECT_EQ(std::get<0>(u), 1);
  EXPECT_EQ(std::get<1>(v), "hello");
}

TEST(VariantTest, GetConst) {
  data::variant<int, std::string> const v{1}, u{std::string{"hello"}};

  EXPECT_EQ(cista::get_if<0>(u), nullptr);
  EXPECT_EQ(cista::get_if<1>(v), nullptr);
  ASSERT_NE(cista::get_if<0>(v), nullptr);
  ASSERT_NE(cista::get_if<1>(u), nullptr);
  EXPECT_EQ(*cista::get_if<0>(v), 1);
  EXPECT_EQ(*cista::get_if<1>(u), std::string{"hello"});

  EXPECT_EQ(std::get<int>(v), 1);
  EXPECT_EQ(std::get<std::string>(u), "hello");
  EXPECT_EQ(std::get<0>(v), 1);
  EXPECT_EQ(std::get<1>(u), "hello");
}

TEST(VariantTest, Serialization) {
  namespace data = cista::offset;
  constexpr auto const MODE =  // opt. versioning + check sum
      cista::mode::WITH_VERSION | cista::mode::WITH_INTEGRITY;

  struct pos {
    int x, y;
  };
  using property = data::variant<data::string, std::int64_t>;
  using pos_map =  // Automatic deduction of hash & equality
      data::hash_map<data::vector<pos>, data::hash_set<property>>;

  {  // Serialize.
    auto positions = pos_map{
        {{{1, 2}, {3, 4}},
         {property{data::string{"hello"}}, property{std::int64_t{123}}}},
        {{{5, 6}, {7, 8}},
         {property{std::int64_t{456}}, property{data::string{"world"}}}}};
    cista::buf mmap{cista::mmap{"data"}};
    cista::serialize<MODE>(mmap, positions);
  }

  // Deserialize.
  auto b = cista::file("data", "r").content();
  auto positions = cista::deserialize<pos_map, MODE>(b);

  // Check.
  ASSERT_NE(positions, nullptr);
  EXPECT_EQ(positions->size(), 2);
  auto const one = positions->find(pos_map::key_type{{1, 2}, {3, 4}});
  auto const two = positions->find(pos_map::key_type{{5, 6}, {7, 8}});
  EXPECT_NE(one, positions->end());
  EXPECT_NE(two, positions->end());
  EXPECT_NE(one->second.find(property{data::string{"hello"}}), end(one->second));
  EXPECT_NE(two->second.find(property{std::int64_t{456}}), end(two->second));

  // Clean up test file
  std::remove("data");
}

TEST(VariantTest, GetIf) {
  namespace CISTA = cista::offset;
  struct Test;

  using Variant =
      CISTA::variant<bool, std::int64_t, std::uint64_t, double, CISTA::string,
                     CISTA::unique_ptr<int>, CISTA::unique_ptr<Test>>;

  struct Test : public CISTA::vector<Variant> {};

  auto t1 = Variant{true};
  auto const result = cista::get_if<bool>(t1);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(*result, true);

  auto const t2 = Variant{CISTA::string{"hello test test test test"}};
  auto const str = cista::get_if<CISTA::string>(t2);
  ASSERT_NE(str, nullptr);
  EXPECT_EQ(*str, "hello test test test test");
}

TEST(VariantTest, StdVisit) {
  std::visit(
      [](auto&& x) {
        if constexpr (std::is_same_v<int, std::decay_t<decltype(x)>>) {
          EXPECT_TRUE(true);
        } else {
          EXPECT_TRUE(false);
        }
      },
      cista::variant<int, float>{1});
}