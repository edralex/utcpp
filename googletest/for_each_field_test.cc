#include "gtest/gtest.h"

#include <sstream>
#include <string>
#include <vector>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/reflection/for_each_field.h"
#endif

struct a {
  static constexpr auto const id = 77;
  int i_ = 1;
  int j_ = 2;
  double d_ = 100.0;
  std::string s_ = "hello world";
};

struct ignore {};

template <typename T>
std::vector<std::string> encode(T&& m) {
  using Type = std::decay_t<T>;
  std::vector<std::string> fields = {std::to_string(Type::id)};
  std::stringstream ss;
  cista::for_each_field(m, [&](auto&& f) {
    if constexpr (!std::is_same_v<cista::decay_t<decltype(f)>, ignore>) {
      ss << f;
      fields.emplace_back(ss.str());
      ss.str("");
    }
  });
  return fields;
}

TEST(ForEachFieldTest, ForEachField) {
  a instance;
  cista::for_each_field(instance, [](auto&& m) { m = {}; });
  EXPECT_EQ(instance.i_, 0);
  EXPECT_EQ(instance.j_, 0);
  EXPECT_EQ(instance.d_, 0.0);
  EXPECT_EQ(instance.s_, "");
  EXPECT_EQ(encode(instance), std::vector<std::string>({"77", "0", "0", "0", ""}));
}

struct current_time_req {
  static constexpr auto const id = 49;
  int version_ = 1;
  ignore s_;
};

TEST(ForEachFieldTest, ForEachField1) {
  current_time_req a;
  EXPECT_EQ(encode(a), std::vector<std::string>({"49", "1"}));
}
