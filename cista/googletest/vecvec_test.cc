#include <array>
#include <memory>
#include <set>
#include <stdexcept>
#include <string_view>

#include "gtest/gtest.h"

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/containers/mmap_vec.h"
#include "cista/containers/paged.h"
#include "cista/containers/paged_vecvec.h"
#include "cista/containers/vecvec.h"
#include "cista/strong.h"
#endif

using namespace std::literals;

TEST(VecVecTest, InsertBeginTest) {
  using key = cista::strong<unsigned, struct x_>;
  struct value {
    double lat_, lng_;
  };
  using data = cista::raw::vecvec<key, value>;

  data d;
  EXPECT_EQ(d.size(), 0U);
  EXPECT_TRUE(d.empty());

  d.emplace_back(
      std::initializer_list<value>{value{1.0, 1.0}, value{2.0, 2.0}});
  EXPECT_EQ(d.size(), 1);
  EXPECT_EQ(d[key{0}].size(), 2);
}

TEST(VecVecTest, StringTest) {
  using key = cista::strong<unsigned, struct x_>;
  using data = cista::raw::vecvec<key, char>;

  data d;
  d.emplace_back("hello");
  EXPECT_EQ(d[key{0U}].view(), "hello");
}

TEST(VecVecTest, SortBucketTest) {
  using key = cista::strong<unsigned, struct x_>;
  using data = cista::raw::vecvec<key, char>;

  data d;
  d.emplace_back("hello");

  std::sort(begin(d.at(key{0})), end(d.at(key{0})));

  EXPECT_EQ("ehllo", d.at(key{0}).view());
}

TEST(VecVecTest, BucketEmplaceBackTest) {
  using key = cista::strong<unsigned, struct x_>;
  using data = cista::raw::vecvec<key, char>;

  data d;
  d.emplace_back("hello");
  d.emplace_back("world");
  d.emplace_back("test");

  d[key{0}].push_back('x');
  EXPECT_EQ("hellox", d[key{0}].view());
  EXPECT_EQ("world", d[key{1}].view());
  EXPECT_EQ("test", d[key{2}].view());

  d[key{1}].push_back('x');
  EXPECT_EQ("hellox", d[key{0}].view());
  EXPECT_EQ("worldx", d[key{1}].view());
  EXPECT_EQ("test", d[key{2}].view());

  d[key{2}].push_back('x');
  EXPECT_EQ("hellox", d[key{0}].view());
  EXPECT_EQ("worldx", d[key{1}].view());
  EXPECT_EQ("testx", d[key{2}].view());
}

TEST(VecVecTest, BucketGrowTest) {
  using key = cista::strong<unsigned, struct x_>;
  using data = cista::raw::vecvec<key, char>;

  data d;
  d.emplace_back("hello");
  d.emplace_back("world");

  {
    d[key{0}].grow(10);
    EXPECT_EQ("hello\0\0\0\0\0"sv, d[key{0}].view());
    EXPECT_EQ("world", d[key{1}].view());
  }

  {
    d[key{0}].grow(13, ' ');
    EXPECT_EQ("hello\0\0\0\0\0   "sv, d[key{0}].view());
    EXPECT_EQ("world", d[key{1}].view());
  }

  EXPECT_THROW(d[key{0}].grow(5), std::runtime_error);
}

TEST(VecVecTest, ResizeTest) {
  using key = cista::strong<unsigned, struct x_>;
  using data = cista::raw::vecvec<key, char>;

  data d;
  d.emplace_back("hello");
  d.emplace_back("world");
  d.emplace_back("test");

  d.resize(5);

  const std::array<std::string_view, 5> expected = {
      "hello"sv, "world"sv, "test"sv, ""sv, ""sv
  };
  for (auto i = 0U; i != d.size(); ++i) {
    EXPECT_EQ(d[key{i}].view(), expected[i]);
  }

  d.resize(1);
  const std::array<std::string_view, 1> expected_1 = {"hello"sv};
  for (auto i = 0U; i != d.size(); ++i) {
    EXPECT_EQ(d[key{i}].view(), expected_1[i]);
  }
}

TEST(VecVecTest, Mmap) {
  using key = cista::strong<unsigned, struct x_>;
  using idx_t = cista::mmap_vec<cista::base_t<key>>;
  using data_t = cista::mmap_vec<char>;

  auto idx = idx_t{cista::mmap{std::tmpnam(nullptr)}};
  auto data = data_t{cista::mmap{std::tmpnam(nullptr)}};
  auto d =
      cista::basic_vecvec<key, data_t, idx_t>{std::move(data), std::move(idx)};

  d.emplace_back("hello");
  d.emplace_back("world");
  d.emplace_back("test");

  EXPECT_EQ("hello", d[key{0}].view());
  EXPECT_EQ("world", d[key{1}].view());
  EXPECT_EQ("test", d[key{2}].view());

  d[key{0}].push_back('x');
  EXPECT_EQ("hellox", d[key{0}].view());
  EXPECT_EQ("world", d[key{1}].view());
  EXPECT_EQ("test", d[key{2}].view());

  d[key{1}].push_back('x');
  EXPECT_EQ("hellox", d[key{0}].view());
  EXPECT_EQ("worldx", d[key{1}].view());
  EXPECT_EQ("test", d[key{2}].view());

  d[key{2}].push_back('x');
  EXPECT_EQ("hellox", d[key{0}].view());
  EXPECT_EQ("worldx", d[key{1}].view());
  EXPECT_EQ("testx", d[key{2}].view());
}

TEST(VecVecTest, PagedVecVecMmap) {
  struct x {
    char x_{'x'};
  };

  auto t = cista::mmap_vec<x>{cista::mmap(std::tmpnam(nullptr))};
  t.push_back(x{'y'});
  EXPECT_EQ(1, t.size());
  t.resize(5);
  EXPECT_EQ(5, t.size());

  bool first = true;
  for (const auto& elem : t) {
    if (first) {
      EXPECT_EQ(elem.x_, 'y');
      first = false;
    } else {
      EXPECT_EQ(elem.x_, 'x');
    }
  }

  using key = cista::strong<unsigned, struct x_>;
  using data_t = cista::paged<cista::mmap_vec<char>>;
  using idx_t = cista::mmap_vec<data_t::page_t>;

  auto idx = idx_t{cista::mmap{std::tmpnam(nullptr)}};
  auto data = data_t{cista::mmap_vec<char>{cista::mmap{std::tmpnam(nullptr)}}};
  auto d =
      cista::paged_vecvec<idx_t, data_t, key>{std::move(data), std::move(idx)};

  d.emplace_back("hello");
  d.emplace_back("world");
  d.emplace_back("test");

  EXPECT_EQ("hello", d[key{0}].view());
  EXPECT_EQ("world", d[key{1}].view());
  EXPECT_EQ("test", d[key{2}].view());

  d[key{0}].push_back('x');
  EXPECT_EQ("hellox", d[key{0}].view());
  EXPECT_EQ("world", d[key{1}].view());
  EXPECT_EQ("test", d[key{2}].view());

  d[key{1}].push_back('x');
  EXPECT_EQ("hellox", d[key{0}].view());
  EXPECT_EQ("worldx", d[key{1}].view());
  EXPECT_EQ("test", d[key{2}].view());

  d[key{2}].push_back('x');
  EXPECT_EQ("hellox", d[key{0}].view());
  EXPECT_EQ("worldx", d[key{1}].view());
  EXPECT_EQ("testx", d[key{2}].view());
}

TEST(VecVecTest, PagedVecVecVector) {
  using key = cista::strong<unsigned, struct x_>;
  using data_t = cista::paged<cista::raw::vector<char>>;
  using idx_t = cista::mmap_vec<data_t::page_t>;

  auto idx = idx_t{cista::mmap{std::tmpnam(nullptr)}};
  auto data = data_t{};
  auto d =
      cista::paged_vecvec<idx_t, data_t, key>{std::move(data), std::move(idx)};

  d.resize(3U);
  EXPECT_EQ(3, d.size());

  d.emplace_back("hello");
  d.emplace_back("world");
  d.emplace_back("test");

  EXPECT_EQ(6, d.size());

  EXPECT_EQ("hello", d[key{3}].view());
  EXPECT_EQ("world", d[key{4}].view());
  EXPECT_EQ("test", d[key{5}].view());

  d[key{3}].push_back('x');
  EXPECT_EQ("hellox", d[key{3}].view());
  EXPECT_EQ("world", d[key{4}].view());
  EXPECT_EQ("test", d[key{5}].view());

  d[key{4}].push_back('x');
  EXPECT_EQ("hellox", d[key{3}].view());
  EXPECT_EQ("worldx", d[key{4}].view());
  EXPECT_EQ("test", d[key{5}].view());

  d[key{5}].push_back('x');
  EXPECT_EQ("hellox", d[key{3}].view());
  EXPECT_EQ("worldx", d[key{4}].view());
  EXPECT_EQ("testx", d[key{5}].view());

  d.resize(0);
  d.emplace_back("hello");
  d.emplace_back("world");
  d.emplace_back("test");

  d[key{0}].push_back('x');
  EXPECT_EQ("hellox", d[key{0}].view());
  EXPECT_EQ("world", d[key{1}].view());
  EXPECT_EQ("test", d[key{2}].view());

  d[key{1}].push_back('x');
  EXPECT_EQ("hellox", d[key{0}].view());
  EXPECT_EQ("worldx", d[key{1}].view());
  EXPECT_EQ("test", d[key{2}].view());

  d[key{2}].push_back('x');
  EXPECT_EQ("hellox", d[key{0}].view());
  EXPECT_EQ("worldx", d[key{1}].view());
  EXPECT_EQ("testx", d[key{2}].view());
}