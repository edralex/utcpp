#include "gtest/gtest.h"

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/containers/hash_map.h"
#include "cista/containers/hash_set.h"
#include "cista/hash.h"
#include "cista/serialization.h"
#endif

TEST(HashSetTest, DeleteEven) {
  auto const max = 250;
  cista::raw::hash_set<int> uut;
  for (auto j = 0; j < 10; ++j) {
    for (auto i = 0; i < max; ++i) {
      auto const res = uut.emplace(i);
      EXPECT_TRUE(res.second);
      EXPECT_EQ(*res.first, i);
      EXPECT_NE(uut.find(i), uut.end());
    }
    EXPECT_EQ(uut.size(), max);
    for (auto i = 0; i < max; ++i) {
      auto const res = uut.emplace(i);
      EXPECT_FALSE(res.second);
      EXPECT_EQ(*res.first, i);
      if (i % 2 == 0) {
        uut.erase(i);
      }
    }
    EXPECT_EQ(uut.size(), max / 2);
    for (auto it = begin(uut); it != end(uut); ++it) {
      EXPECT_NE(*it % 2, 0);
      EXPECT_EQ(*uut.find(*it), *it);
      uut.erase(it);
    }
    EXPECT_TRUE(uut.empty());
  }
}

TEST(HashSetTest, DeleteHalf) {
  auto const max = 250;
  cista::raw::hash_set<int> uut;
  for (auto j = 0; j < 10; ++j) {
    for (auto i = 0; i < max; ++i) {
      auto const res = uut.emplace(i);
      EXPECT_TRUE(res.second);
      EXPECT_EQ(*res.first, i);
      EXPECT_NE(uut.find(i), uut.end());
    }
    EXPECT_EQ(uut.size(), max);
    for (auto i = 0; i < max; ++i) {
      auto const res = uut.emplace(i);
      EXPECT_FALSE(res.second);
      EXPECT_EQ(*res.first, i);
      if (i >= max / 2) {
        uut.erase(i);
      }
    }
    EXPECT_EQ(uut.size(), max / 2);
    for (auto it = begin(uut); it != end(uut); ++it) {
      EXPECT_LT(*it, max / 2);
      EXPECT_EQ(*uut.find(*it), *it);
      uut.erase(it);
    }
    EXPECT_TRUE(uut.empty());
  }
}

TEST(HashMapTest, HashMap) {
  auto const max = 250;
  cista::raw::hash_map<int, int> uut;
  for (auto i = 0; i < max; ++i) {
    auto const res = uut.emplace(i, i + 1);
    EXPECT_TRUE(res.second);
    EXPECT_EQ(res.first->first, i);
    EXPECT_EQ(res.first->second, i + 1);
    EXPECT_NE(uut.find(i), uut.end());
  }
  for (auto i = 0; i < max; ++i) {
    auto const res = uut.emplace(i, i + 1);
    EXPECT_FALSE(res.second);
    EXPECT_EQ(res.first->first, i);
    EXPECT_EQ(res.first->second, i + 1);
    if (i % 2 == 0) {
      uut.erase(i);
    }
  }
  for (auto it = begin(uut); it != end(uut); ++it) {
    EXPECT_NE(it->first % 2, 0);
    EXPECT_EQ(uut.find(it->first)->first, it->first);
    EXPECT_EQ(uut.find(it->first)->second, it->second);
    uut.erase(it);
  }
  EXPECT_TRUE(uut.empty());
}

TEST(HashSetTest, IterateEmpty) {
  using namespace cista::raw;
  hash_set<vector<string>> v;
  for (auto& e : v) {
    (void)e;
    EXPECT_TRUE(false);
  }
}

TEST(HashSetTest, Serialize) {
  using namespace cista;
  using namespace cista::raw;

  auto const make_e1 = []() {
    vector<string> e1;
    e1.emplace_back("short");
    e1.emplace_back("long long long long long long long");
    return e1;
  };

  auto const make_e2 = []() {
    vector<string> e2;
    e2.emplace_back("hello");
    e2.emplace_back("world");
    e2.emplace_back("yeah!");
    return e2;
  };

  auto const make_e3 = []() {
    vector<string> e3;
    e3.emplace_back("This");
    e3.emplace_back("is");
    e3.emplace_back("Sparta");
    e3.emplace_back("!!!");
    return e3;
  };

  using serialize_me_t = hash_set<vector<string>>;

  byte_buf buf;

  {
    serialize_me_t s;
    s.emplace(make_e1());
    s.emplace(make_e2());
    s.emplace(make_e3());
    buf = serialize(s);
  }  // EOL s

  auto const deserialized = deserialize<serialize_me_t>(buf);

  EXPECT_EQ(deserialized->size(), 3U);

  EXPECT_NE(deserialized->find(make_e1()), deserialized->end());
  EXPECT_NE(deserialized->find(make_e2()), deserialized->end());
  EXPECT_NE(deserialized->find(make_e3()), deserialized->end());

  EXPECT_EQ(*deserialized->find(make_e1()), make_e1());
  EXPECT_EQ(*deserialized->find(make_e2()), make_e2());
  EXPECT_EQ(*deserialized->find(make_e3()), make_e3());
}

TEST(HashSetTest, SelfAssignment) {
  using namespace cista::raw;

  hash_set<string> s{"hash", "set", "self", "assignment"};
  s = s;

  EXPECT_EQ(s.size(), 4);
  EXPECT_NE(s.find("hash"), s.end());
  EXPECT_NE(s.find("set"), s.end());
  EXPECT_NE(s.find("self"), s.end());
  EXPECT_NE(s.find("assignment"), s.end());
}

TEST(HashMapTest, SelfAssignment) {
  using namespace cista::raw;

  hash_map<string, int> s{
      {"hash", 1}, {"map", 2}, {"self", 3}, {"assignment", 128}};
  s = s;

  EXPECT_EQ(s.size(), 4);
  EXPECT_NE(s.find("hash"), s.end());
  EXPECT_NE(s.find("map"), s.end());
  EXPECT_NE(s.find("self"), s.end());
  EXPECT_NE(s.find("assignment"), s.end());

  EXPECT_EQ(s["hash"], 1);
  EXPECT_EQ(s["map"], 2);
  EXPECT_EQ(s["self"], 3);
  EXPECT_EQ(s["assignment"], 128);
}

#ifndef _MSC_VER  // MSVC compiler bug :/
TEST(HashMapTest, StringViewGet) {
  using namespace cista::raw;

  hash_map<std::string, int> s;

  s[{"0"}] = 0;  // Calls std::string constructor with key_t const& overload
  s[string{"1"}] = 1;  // cista::string to std::string_view to std::string
  s["2"] = 2;  // Calls std::string constructor on insertion

  // operator[]
  EXPECT_EQ(s[{"0"}], 0);
  EXPECT_EQ(s[string{"0"}], 0);
  EXPECT_EQ(s["0"], 0);

  EXPECT_EQ(s[{"1"}], 1);
  EXPECT_EQ(s[string{"1"}], 1);
  EXPECT_EQ(s["1"], 1);

  EXPECT_EQ(s[{"2"}], 2);
  EXPECT_EQ(s[string{"2"}], 2);
  EXPECT_EQ(s["2"], 2);

  // .at()
  EXPECT_EQ(s.at({"0"}), 0);
  EXPECT_EQ(s.at(string{"0"}), 0);
  EXPECT_EQ(s.at("0"), 0);

  EXPECT_EQ(s.at({"1"}), 1);
  EXPECT_EQ(s.at(string{"1"}), 1);
  EXPECT_EQ(s.at("1"), 1);

  EXPECT_EQ(s.at({"2"}), 2);
  EXPECT_EQ(s.at(string{"2"}), 2);
  EXPECT_EQ(s.at("2"), 2);

  // .find()
  EXPECT_EQ(s.find({"0"})->second, 0);
  EXPECT_EQ(s.find(string{"0"})->second, 0);
  EXPECT_EQ(s.find("0")->second, 0);

  EXPECT_EQ(s.find({"1"})->second, 1);
  EXPECT_EQ(s.find(string{"1"})->second, 1);
  EXPECT_EQ(s.find("1")->second, 1);

  EXPECT_EQ(s.find({"2"})->second, 2);
  EXPECT_EQ(s.find(string{"2"})->second, 2);
  EXPECT_EQ(s.find("2")->second, 2);

  // .erase()
  EXPECT_EQ(s.erase({"0"}), 1);
  EXPECT_EQ(s.erase(string{"0"}), 0);
  EXPECT_EQ(s.erase("0"), 0);

  EXPECT_EQ(s.erase({"1"}), 1);
  EXPECT_EQ(s.erase(string{"1"}), 0);
  EXPECT_EQ(s.erase("1"), 0);

  EXPECT_EQ(s.erase({"2"}), 1);
  EXPECT_EQ(s.erase(string{"2"}), 0);
  EXPECT_EQ(s.erase("2"), 0);
}
#endif
