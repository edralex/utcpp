#include <iostream>

#include "gtest/gtest.h"

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/containers/cstring.h"
#include "cista/hash.h"
#endif

using cista::raw::cstring;

constexpr auto const CORNER_CASE_SHORT_14 = "01234567891234";
constexpr auto const CORNER_CASE_SHORT_15 = "012345678912345";
constexpr auto const CORNER_CASE_LONG_16 = "0123456789123456";
constexpr auto const LONG_STR = "hello world hello world";
constexpr auto const SHORT_STR = "hello world";

TEST(CStringTest, Init) {
  auto s = cstring{};
  EXPECT_TRUE(s.is_short());
  EXPECT_EQ(s.size(), 0);
  EXPECT_NE(s.data(), nullptr);
}

TEST(CStringTest, LongShortCorner14) {
  auto s = cstring{CORNER_CASE_SHORT_14, cstring::owning};
  EXPECT_TRUE(s.is_short());
  EXPECT_EQ(s.size(), std::strlen(CORNER_CASE_SHORT_14));
  EXPECT_EQ(s.view(), CORNER_CASE_SHORT_14);
}

TEST(CStringTest, LongShortCorner15) {
  auto s = cstring{CORNER_CASE_SHORT_15, cstring::owning};
  EXPECT_TRUE(s.is_short());
  EXPECT_EQ(s.size(), std::strlen(CORNER_CASE_SHORT_15));
  EXPECT_EQ(s.view(), CORNER_CASE_SHORT_15);
}

TEST(CStringTest, LongShortCorner16) {
  auto s = cstring{CORNER_CASE_LONG_16, cstring::owning};
  EXPECT_FALSE(s.is_short());
  EXPECT_EQ(s.size(), std::strlen(CORNER_CASE_LONG_16));
  EXPECT_EQ(s.view(), CORNER_CASE_LONG_16);
}

TEST(CStringTest, LongShort) {
  auto s = cstring{SHORT_STR, cstring::owning};
  EXPECT_EQ(s.view(), SHORT_STR);
  EXPECT_TRUE(s.is_short());

  s.set_owning(CORNER_CASE_LONG_16);
  EXPECT_FALSE(s.is_short());
  EXPECT_EQ(s.view(), CORNER_CASE_LONG_16);

  s.set_owning(LONG_STR);
  EXPECT_FALSE(s.is_short());
  EXPECT_EQ(s.view(), LONG_STR);
}

TEST(CStringTest, DeallocLongToShort) {
  cstring s = "one two";
  EXPECT_EQ(s.size(), std::strlen("one two"));
  EXPECT_TRUE(s.is_short());
  s.set_non_owning("");
}

TEST(CStringTest, CopyAssignAndCopyConstruct) {
  auto s0 = cstring{LONG_STR, cstring::owning};
  auto s1 = cstring{s0};
  EXPECT_EQ(s0, s1);
  EXPECT_EQ(s1.view(), LONG_STR);

  cstring s2;
  s2 = s0;
  EXPECT_EQ(s0, s2);
  EXPECT_EQ(s2.view(), LONG_STR);
}

TEST(CStringTest, Hash) {
  auto str = cstring{""};
  auto h = cista::hash(str, cista::BASE_HASH);
  EXPECT_EQ(cista::BASE_HASH, h);
}
