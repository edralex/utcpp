#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <gtest/gtest.h>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/containers/string.h"
#include "cista/hash.h"
#endif

using cista::raw::string;

constexpr auto const CORNER_CASE_SHORT_14 = "01234567891234";
constexpr auto const CORNER_CASE_SHORT_15 = "012345678912345";
constexpr auto const CORNER_CASE_LONG_16 = "0123456789123456";
constexpr auto const LONG_STR = "hello world hello world";
constexpr auto const SHORT_STR = "hello world";

// === char string tests ===

TEST(StringTest, SelfAssignment) {
  // Short string
  cista::raw::string test_short{"test_short"};
  std::string output_short_pre{test_short};
  test_short = test_short;
  std::string output_short_post{test_short};
  EXPECT_EQ(output_short_pre, output_short_post);

  // Long string
  cista::raw::string test_long{"test_long_12345678901234567890123456789012"};
  std::string output_long_pre{test_long};
  test_long = test_long;
  std::string output_long_post{test_long};
  EXPECT_EQ(output_long_pre, output_long_post);
}

TEST(StringTest, StringInit) {
  auto s = string{};
  EXPECT_FALSE(s.is_short());
  EXPECT_EQ(s.size(), 0U);
  EXPECT_EQ(s.data(), nullptr);
}

TEST(StringTest, StringLongShortCorner14) {
  auto s = string{CORNER_CASE_SHORT_14, string::owning};
  EXPECT_TRUE(s.is_short());
  EXPECT_EQ(s.size(), std::strlen(CORNER_CASE_SHORT_14));
  EXPECT_EQ(s.view(), CORNER_CASE_SHORT_14);
}

TEST(StringTest, StringErase) {
  auto uut = std::vector<cista::generic_string<char const*>>{};
  auto ref = std::vector<std::string>{};
  for (auto const s :
       {CORNER_CASE_SHORT_14, CORNER_CASE_SHORT_15, CORNER_CASE_LONG_16}) {
    auto x = cista::generic_string{s, cista::generic_string<char const*>::non_owning};
    x.erase(3, 7);
    uut.emplace_back(std::move(x));

    auto y = std::string{s};
    y.erase(3, 7);
    ref.emplace_back(std::move(y));
  }

  for (size_t i = 0; i != ref.size(); ++i) {
    EXPECT_EQ(ref[i], uut[i].view());
  }
}

TEST(StringTest, StringLongShortCorner15) {
  auto s = string{CORNER_CASE_SHORT_15, string::owning};
  EXPECT_TRUE(s.is_short());
  EXPECT_EQ(s.size(), std::strlen(CORNER_CASE_SHORT_15));
  EXPECT_EQ(s.view(), CORNER_CASE_SHORT_15);
}

TEST(StringTest, StringLongShortCorner16) {
  auto s = string{CORNER_CASE_LONG_16, string::owning};
  EXPECT_FALSE(s.is_short());
  EXPECT_EQ(s.size(), std::strlen(CORNER_CASE_LONG_16));
  EXPECT_EQ(s.view(), CORNER_CASE_LONG_16);
}

TEST(StringTest, StringLongShort) {
  auto s = string{SHORT_STR, string::owning};
  EXPECT_EQ(s.view(), SHORT_STR);
  EXPECT_TRUE(s.is_short());

  s.set_owning(CORNER_CASE_LONG_16);
  EXPECT_FALSE(s.is_short());
  EXPECT_EQ(s.view(), CORNER_CASE_LONG_16);

  s.set_owning(LONG_STR);
  EXPECT_FALSE(s.is_short());
  EXPECT_EQ(s.view(), LONG_STR);
}

TEST(StringTest, StringDeallocLongToShort) {
  string s = "one two";
  EXPECT_EQ(s.size(), std::strlen("one two"));
  EXPECT_TRUE(s.is_short());
  s.set_non_owning("");
  // No assertion needed — just checks no crash
}

TEST(StringTest, StringCopyAssignAndCopyConstruct) {
  auto s0 = string{LONG_STR, string::owning};
  auto s1 = string{s0};
  EXPECT_EQ(s0, s1);
  EXPECT_EQ(s1.view(), LONG_STR);

  string s2;
  s2 = s0;
  EXPECT_EQ(s0, s2);
  EXPECT_EQ(s2.view(), LONG_STR);
}

TEST(StringTest, StringHash) {
  auto str = string{""};
  auto h = cista::hash(str, cista::BASE_HASH);
  EXPECT_EQ(cista::BASE_HASH, h);
}

TEST(StringTest, StringStartsWith) {
  string s = "abacaba";

  EXPECT_TRUE(s.starts_with("abac"));
  EXPECT_TRUE(s.starts_with("abacaba"));
  EXPECT_FALSE(s.starts_with("abacaba_"));
  EXPECT_TRUE(s.starts_with("a"));
  EXPECT_TRUE(s.starts_with(""));
  EXPECT_FALSE(s.starts_with("abad"));

  EXPECT_TRUE(s.starts_with(string{"abac"}));
  EXPECT_TRUE(s.starts_with(string{"abacaba"}));
  EXPECT_FALSE(s.starts_with(string{"abacaba_"}));
  EXPECT_TRUE(s.starts_with(string{"a"}));
  EXPECT_TRUE(s.starts_with(string{""}));
  EXPECT_FALSE(s.starts_with(string{"abad"}));

  EXPECT_TRUE(s.starts_with(std::string{"abac"}));
  EXPECT_TRUE(s.starts_with(std::string{"abacaba"}));
  EXPECT_FALSE(s.starts_with(std::string{"abacaba_"}));
  EXPECT_TRUE(s.starts_with(std::string{"a"}));
  EXPECT_TRUE(s.starts_with(std::string{""}));
  EXPECT_FALSE(s.starts_with(std::string{"\0", 1}));
  EXPECT_FALSE(s.starts_with(std::string{"abad"}));

  EXPECT_TRUE(s.starts_with(std::string_view{"abac"}));
  EXPECT_TRUE(s.starts_with(std::string_view{"abacaba"}));
  EXPECT_FALSE(s.starts_with(std::string_view{"abacaba_"}));
  EXPECT_TRUE(s.starts_with(std::string_view{"a"}));
  EXPECT_TRUE(s.starts_with(std::string_view{""}));
  EXPECT_FALSE(s.starts_with(std::string_view{"\0", 1}));
  EXPECT_FALSE(s.starts_with(std::string_view{"abad"}));

  EXPECT_TRUE(s.starts_with('a'));
  EXPECT_FALSE(s.starts_with('b'));
  EXPECT_FALSE(s.starts_with('\0'));
}

TEST(StringTest, StringEndsWith) {
  string s = "abacaba";

  EXPECT_TRUE(s.ends_with("caba"));
  EXPECT_TRUE(s.ends_with("abacaba"));
  EXPECT_FALSE(s.ends_with("abacaba_"));
  EXPECT_TRUE(s.ends_with("a"));
  EXPECT_TRUE(s.ends_with(""));
  EXPECT_FALSE(s.ends_with("daba"));

  EXPECT_TRUE(s.ends_with(string{"caba"}));
  EXPECT_TRUE(s.ends_with(string{"abacaba"}));
  EXPECT_FALSE(s.ends_with(string{"abacaba_"}));
  EXPECT_TRUE(s.ends_with(string{"a"}));
  EXPECT_TRUE(s.ends_with(string{""}));
  EXPECT_FALSE(s.ends_with(string{"daba"}));

  EXPECT_TRUE(s.ends_with(std::string{"caba"}));
  EXPECT_TRUE(s.ends_with(std::string{"abacaba"}));
  EXPECT_FALSE(s.ends_with(std::string{"abacaba_"}));
  EXPECT_TRUE(s.ends_with(std::string{"a"}));
  EXPECT_TRUE(s.ends_with(std::string{""}));
  EXPECT_FALSE(s.ends_with(std::string{"\0", 1}));
  EXPECT_FALSE(s.ends_with(std::string{"daba"}));

  EXPECT_TRUE(s.ends_with(std::string_view{"caba"}));
  EXPECT_TRUE(s.ends_with(std::string_view{"abacaba"}));
  EXPECT_FALSE(s.ends_with(std::string_view{"abacaba_"}));
  EXPECT_TRUE(s.ends_with(std::string_view{"a"}));
  EXPECT_TRUE(s.ends_with(std::string_view{""}));
  EXPECT_FALSE(s.ends_with(std::string_view{"\0", 1}));
  EXPECT_FALSE(s.ends_with(std::string_view{"daba"}));

  EXPECT_TRUE(s.ends_with('a'));
  EXPECT_FALSE(s.ends_with('b'));
  EXPECT_FALSE(s.ends_with('\0'));
}

// === char16_t string tests ===

using cista::raw::u16string;
using cista::raw::u16string_view;

const auto constexpr U16STR_SHORT = u"0123";
const auto constexpr U16STR_SHORT_CORNER_CASE = u"0123456";
const auto constexpr U16STR_LONG_CORNER_CASE = u"01234567";
const auto constexpr U16STR_LONG = u"0123456789ABCDEF\xD83D\xDCBB";

TEST(U16StringTest, LongShort) {
  u16string s[] = {u"", U16STR_SHORT, U16STR_SHORT_CORNER_CASE,
                   U16STR_LONG_CORNER_CASE, U16STR_LONG};

  EXPECT_FALSE(s[0].is_short());
  EXPECT_EQ(s[0].size(), 0U);
  EXPECT_EQ(s[0].data(), nullptr);

  EXPECT_TRUE(s[1].is_short());
  EXPECT_EQ(s[1].size(), 4U);
  EXPECT_EQ(s[1].view(), U16STR_SHORT);

  EXPECT_TRUE(s[2].is_short());
  EXPECT_EQ(s[2].size(), 7U);
  EXPECT_EQ(s[2].view(), U16STR_SHORT_CORNER_CASE);

  EXPECT_FALSE(s[3].is_short());
  EXPECT_EQ(s[3].size(), 8U);
  EXPECT_EQ(s[3].view(), U16STR_LONG_CORNER_CASE);

  EXPECT_FALSE(s[4].is_short());
  EXPECT_EQ(s[4].size(), 18U);
  EXPECT_EQ(s[4].view(), U16STR_LONG);
}

TEST(U16StringTest, Copy) {
  u16string s, s_s{U16STR_SHORT}, s_l{U16STR_LONG};
  u16string_view sv_s{U16STR_SHORT}, sv_l{U16STR_LONG};

  s = U16STR_SHORT;
  EXPECT_EQ(s.view(), U16STR_SHORT);

  s = U16STR_LONG;
  EXPECT_EQ(s.view(), U16STR_LONG);

  s = s_s;
  EXPECT_EQ(s, s_s);
  EXPECT_EQ(s.view(), U16STR_SHORT);

  s = s_l;
  EXPECT_EQ(s, s_l);
  EXPECT_EQ(s.view(), U16STR_LONG);

  s = sv_s;
  EXPECT_EQ(s, sv_s);
  EXPECT_EQ(s.view(), U16STR_SHORT);

  s = sv_l;
  EXPECT_EQ(s, sv_l);
  EXPECT_EQ(s.view(), U16STR_LONG);
}

TEST(U16StringTest, Erase) {
  u16string s[] = {U16STR_SHORT, U16STR_SHORT_CORNER_CASE,
                   U16STR_LONG_CORNER_CASE, U16STR_LONG};
  std::u16string ref[] = {U16STR_SHORT, U16STR_SHORT_CORNER_CASE,
                          U16STR_LONG_CORNER_CASE, U16STR_LONG};

  for (u16string::msize_t i = 0; i < 4; ++i) {
    s[i].erase(1, i + 2);
    ref[i].erase(1, i + 2);
    EXPECT_EQ(s[i], ref[i]);
  }

  for (u16string::msize_t i = 0; i < 4; ++i) {
    s[i].erase(0, s[i].size());
    EXPECT_TRUE(s[i].empty());
  }
}

TEST(U16StringTest, StartsWith) {
  u16string s[] = {U16STR_SHORT, U16STR_SHORT_CORNER_CASE,
                   U16STR_LONG_CORNER_CASE, U16STR_LONG};

  for (int i = 1; i < 4; ++i) {
    EXPECT_TRUE(s[i].starts_with(s[i - 1]));
  }
  for (int i = 1; i < 4; ++i) {
    EXPECT_FALSE(s[i - 1].starts_with(s[i]));
  }
  for (int i = 0; i < 4; ++i) {
    EXPECT_TRUE(s[i].starts_with(u'0'));
    EXPECT_FALSE(s[i].starts_with(u'A'));
  }
}

TEST(U16StringTest, EndsWith) {
  u16string s[] = {U16STR_SHORT, U16STR_SHORT_CORNER_CASE,
                   U16STR_LONG_CORNER_CASE, U16STR_LONG};

  EXPECT_TRUE(s[0].ends_with(U16STR_SHORT));
  EXPECT_TRUE(s[0].ends_with(u"123"));
  EXPECT_TRUE(s[0].ends_with(u'3'));
  EXPECT_FALSE(s[0].ends_with(u"01234"));
  EXPECT_FALSE(s[0].ends_with(u'A'));

  EXPECT_TRUE(s[1].ends_with(U16STR_SHORT_CORNER_CASE));
  EXPECT_TRUE(s[1].ends_with(u"456"));
  EXPECT_TRUE(s[1].ends_with(u'6'));
  EXPECT_FALSE(s[1].ends_with(u"01234567"));
  EXPECT_FALSE(s[1].ends_with(u'A'));

  EXPECT_TRUE(s[2].ends_with(U16STR_LONG_CORNER_CASE));
  EXPECT_TRUE(s[2].ends_with(u"567"));
  EXPECT_TRUE(s[2].ends_with(u'7'));
  EXPECT_FALSE(s[2].ends_with(u"012345677"));
  EXPECT_FALSE(s[2].ends_with(u'A'));

  EXPECT_TRUE(s[3].ends_with(U16STR_LONG));
  EXPECT_TRUE(s[3].ends_with(u"F\xD83D\xDCBB"));
  EXPECT_TRUE(s[3].ends_with(u'\xDCBB'));
  EXPECT_FALSE(s[3].ends_with(u'A'));
}

// === char32_t string tests ===

using cista::raw::u32string;
using cista::raw::u32string_view;

const auto constexpr U32STR_SHORT = U"01";
const auto constexpr U32STR_SHORT_CORNER_CASE = U"012";
const auto constexpr U32STR_LONG_CORNER_CASE = U"0123";
const auto constexpr U32STR_LONG = U"0123456789ABCDEF\U0001F4BB";

TEST(U32StringTest, LongShort) {
  u32string s[] = {U"", U32STR_SHORT, U32STR_SHORT_CORNER_CASE,
                   U32STR_LONG_CORNER_CASE, U32STR_LONG};

  EXPECT_FALSE(s[0].is_short());
  EXPECT_EQ(s[0].size(), 0U);
  EXPECT_EQ(s[0].data(), nullptr);

  EXPECT_TRUE(s[1].is_short());
  EXPECT_EQ(s[1].size(), 2U);
  EXPECT_EQ(s[1].view(), U32STR_SHORT);

  EXPECT_TRUE(s[2].is_short());
  EXPECT_EQ(s[2].size(), 3U);
  EXPECT_EQ(s[2].view(), U32STR_SHORT_CORNER_CASE);

  EXPECT_FALSE(s[3].is_short());
  EXPECT_EQ(s[3].size(), 4U);
  EXPECT_EQ(s[3].view(), U32STR_LONG_CORNER_CASE);

  EXPECT_FALSE(s[4].is_short());
  EXPECT_EQ(s[4].size(), 17U);
  EXPECT_EQ(s[4].view(), U32STR_LONG);
}

TEST(U32StringTest, Copy) {
  u32string s, s_s{U32STR_SHORT}, s_l{U32STR_LONG};
  u32string_view sv_s{U32STR_SHORT}, sv_l{U32STR_LONG};

  s = U32STR_SHORT;
  EXPECT_EQ(s.view(), U32STR_SHORT);

  s = U32STR_LONG;
  EXPECT_EQ(s.view(), U32STR_LONG);

  s = s_s;
  EXPECT_EQ(s, s_s);
  EXPECT_EQ(s.view(), U32STR_SHORT);

  s = s_l;
  EXPECT_EQ(s, s_l);
  EXPECT_EQ(s.view(), U32STR_LONG);

  s = sv_s;
  EXPECT_EQ(s, sv_s);
  EXPECT_EQ(s.view(), U32STR_SHORT);

  s = sv_l;
  EXPECT_EQ(s, sv_l);
  EXPECT_EQ(s.view(), U32STR_LONG);
}

TEST(U32StringTest, Erase) {
  u32string s[] = {U32STR_SHORT, U32STR_SHORT_CORNER_CASE,
                   U32STR_LONG_CORNER_CASE, U32STR_LONG};
  std::u32string ref[] = {U32STR_SHORT, U32STR_SHORT_CORNER_CASE,
                          U32STR_LONG_CORNER_CASE, U32STR_LONG};

  for (u32string::msize_t i = 0; i < 4; ++i) {
    s[i].erase(1, i + 1);
    ref[i].erase(1, i + 1);
    EXPECT_EQ(s[i], ref[i]);
  }

  for (u32string::msize_t i = 0; i < 4; ++i) {
    s[i].erase(0, s[i].size());
    EXPECT_TRUE(s[i].empty());
  }
}

TEST(U32StringTest, StartsWith) {
  u32string s[] = {U32STR_SHORT, U32STR_SHORT_CORNER_CASE,
                   U32STR_LONG_CORNER_CASE, U32STR_LONG};

  for (int i = 1; i < 4; ++i) {
    EXPECT_TRUE(s[i].starts_with(s[i - 1]));
  }
  for (int i = 1; i < 4; ++i) {
    EXPECT_FALSE(s[i - 1].starts_with(s[i]));
  }
  for (int i = 0; i < 4; ++i) {
    EXPECT_TRUE(s[i].starts_with(U'0'));
    EXPECT_FALSE(s[i].starts_with(U'A'));
  }
}

TEST(U32StringTest, EndsWith) {
  u32string s[] = {U32STR_SHORT, U32STR_SHORT_CORNER_CASE,
                   U32STR_LONG_CORNER_CASE, U32STR_LONG};

  EXPECT_TRUE(s[0].ends_with(U32STR_SHORT));
  EXPECT_TRUE(s[0].ends_with(U'1'));
  EXPECT_FALSE(s[0].ends_with(U"012"));
  EXPECT_FALSE(s[0].ends_with(U'A'));

  EXPECT_TRUE(s[1].ends_with(U32STR_SHORT_CORNER_CASE));
  EXPECT_TRUE(s[1].ends_with(U"12"));
  EXPECT_TRUE(s[1].ends_with(U'2'));
  EXPECT_FALSE(s[1].ends_with(U"0123"));
  EXPECT_FALSE(s[1].ends_with(U'A'));

  EXPECT_TRUE(s[2].ends_with(U32STR_LONG_CORNER_CASE));
  EXPECT_TRUE(s[2].ends_with(U"123"));
  EXPECT_TRUE(s[2].ends_with(U'3'));
  EXPECT_FALSE(s[2].ends_with(U"01234"));
  EXPECT_FALSE(s[2].ends_with(U'A'));

  EXPECT_TRUE(s[3].ends_with(U32STR_LONG));
  EXPECT_TRUE(s[3].ends_with(U"EF\U0001F4BB"));
  EXPECT_TRUE(s[3].ends_with(U'\U0001F4BB'));
  EXPECT_FALSE(s[3].ends_with(U'A'));
}