#include <queue>
#include <set>
#include <string>

#include <gtest/gtest.h>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/containers/hash_map.h"
#include "cista/containers/string.h"
#include "cista/containers/vector.h"
#include "cista/reflection/comparable.h"
#endif

namespace data = cista::raw;

struct hash_override {
  cista::hash_t hash() const { return i_; }
  unsigned i_;
};

struct std_hash_key {
  explicit std_hash_key(unsigned i) : i_{i} {}
  unsigned i_;
};

struct key {
  int i_;
  data::string s_;
};

namespace std {
template <>
class hash<std_hash_key> {
public:
  size_t operator()(std_hash_key const& s) const { return s.i_; }
};
}  // namespace std

struct my_type {
  CISTA_COMPARABLE()
  int code_{};
  std::string text_;
  std::string type_;
};

TEST(HashingTest, HashingStdStringMember) {
  my_type k{3, std::string{"4321"}, std::string{"1234"}};
  auto h1 = cista::hashing<my_type>{}(k);
  auto h2 = cista::hash(
      std::string{"1234"},
      cista::hash(
          std::string{"4321"},
          cista::hash_combine(cista::BASE_HASH, 3)));
  EXPECT_EQ(h1, h2);
}

TEST(HashingTest, HashOverride) {
  hash_override k{7};
  auto h1 = cista::hashing<hash_override>{}(k);
  auto h2 = cista::hash_combine(cista::BASE_HASH, 7);
  EXPECT_EQ(h1, h2);
}

TEST(HashingTest, AutomaticHashValidation) {
  key k{3U, data::string{"1234"}};
  auto h1 = cista::hashing<key>{}(k);
  auto h2 =
      cista::hash(data::string{"1234"},
                  cista::hash_combine(cista::BASE_HASH, 3U));
  EXPECT_EQ(h1, h2);
}

TEST(HashMapTest, AutomaticHashingAndEqualityCheck) {
  data::hash_map<data::vector<key>, int> m{
      {data::vector<key>{{-2, std::to_string(-2)},
                         {-1, std::to_string(-1)},
                         {0, std::to_string(0)}},
       1},
      {data::vector<key>{{-1, std::to_string(-1)},
                         {0, std::to_string(0)},
                         {1, std::to_string(1)}},
       2}};

  for (auto i = 0; i <= 100; ++i) {
    m.emplace(data::vector<key>{{i, std::to_string(i)},
                                {i + 1, std::to_string(i + 1)},
                                {i + 2, std::to_string(i + 2)}},
              i + 3);
  }

  EXPECT_EQ(m.size(), 103U);

  data::vector<key> query{
      {100, std::to_string(100)},
      {101, std::to_string(101)},
      {102, std::to_string(102)}};

  EXPECT_EQ(m[query], 103);
  EXPECT_EQ(m.at(query), 103);

  for (auto const& [k, v] : m) {
    EXPECT_EQ(k.size(), 3U);
    EXPECT_EQ(k.at(0).i_, k.at(0).i_);
    EXPECT_EQ(k.at(0).i_, std::stoi(k.at(0).s_.str()));
    EXPECT_EQ(k.at(1).i_, k.at(1).i_);
    EXPECT_EQ(k.at(1).i_, std::stoi(k.at(1).s_.str()));
    EXPECT_EQ(k.at(2).i_, k.at(2).i_);
    EXPECT_EQ(k.at(2).i_, std::stoi(k.at(2).s_.str()));
    EXPECT_EQ(k.at(0).i_, v - 3);
  }
}