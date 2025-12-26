#include "gtest/gtest.h"

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/serialization.h"
#endif

constexpr auto const LONG_STR = "aaahelloworldtestaaa";
constexpr auto const SHORT_STR = "ahelloworldtest";

TEST(CStringSerializationTest, LongStr) {
  cista::raw::string s = LONG_STR;

  cista::byte_buf buf = cista::serialize(s);
  auto const serialized =
      cista::deserialize<cista::raw::string>(&buf[0], &buf[0] + buf.size());
  EXPECT_EQ(*serialized, std::string_view{LONG_STR});
}

TEST(CStringSerializationTest, ShortStr) {
  cista::raw::string s = SHORT_STR;

  cista::byte_buf buf = cista::serialize(s);
  EXPECT_EQ(buf.size(), sizeof(cista::raw::string));

  auto const serialized =
      cista::deserialize<cista::raw::string>(&buf[0], &buf[0] + buf.size());
  EXPECT_EQ(*serialized, std::string_view{SHORT_STR});
}

using cista::raw::u16string;
using cista::raw::u16string_view;

const auto constexpr U16STR_SHORT = u"0123";
const auto constexpr U16STR_SHORT_CORNER_CASE = u"0123456";
const auto constexpr U16STR_LONG_CORNER_CASE = u"01234567";
const auto constexpr U16STR_LONG = u"0123456789ABCDEF\xD83D\xDCBB";

TEST(U16StringSerializationTest, Short) {
  u16string s = U16STR_SHORT;

  cista::byte_buf buf = cista::serialize(s);
  EXPECT_EQ(buf.size(), sizeof(u16string));

  auto const serialized = cista::deserialize<u16string>(buf);
  EXPECT_EQ(*serialized, U16STR_SHORT);
}

TEST(U16StringSerializationTest, Long) {
  u16string s = U16STR_LONG;

  cista::byte_buf buf = cista::serialize(s);
  EXPECT_EQ(buf.size(), sizeof(u16string) + s.size() * sizeof(char16_t));

  auto const serialized = cista::deserialize<u16string>(buf);
  EXPECT_EQ(*serialized, U16STR_LONG);
}

TEST(U16StringSerializationTest, WithAdditionalChecks) {
  u16string s_s = U16STR_SHORT_CORNER_CASE, s_l = U16STR_LONG_CORNER_CASE;
  constexpr auto mode = cista::mode::WITH_VERSION |
                        cista::mode::WITH_INTEGRITY | cista::mode::DEEP_CHECK;

  cista::byte_buf buf_s = cista::serialize<mode>(s_s),
                  buf_l = cista::serialize<mode>(s_l);
  u16string *serialized_s, *serialized_l;

  EXPECT_NO_THROW((serialized_s = cista::deserialize<u16string, mode>(buf_s)));
  EXPECT_EQ(*serialized_s, U16STR_SHORT_CORNER_CASE);
  EXPECT_NO_THROW((serialized_l = cista::deserialize<u16string, mode>(buf_l)));
  EXPECT_EQ(*serialized_l, U16STR_LONG_CORNER_CASE);
}

TEST(U16StringSerializationTest, EndianShort) {
  cista::byte_buf str_le = {0x30, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33,
                            0x00, 0x34, 0x00, 0x35, 0x00, 0x36, 0x00};
  cista::byte_buf str_be = {0x00, 0x30, 0x00, 0x31, 0x00, 0x32, 0x00,
                            0x33, 0x00, 0x34, 0x00, 0x35, 0x00, 0x36};
  u16string s = U16STR_SHORT_CORNER_CASE;

  cista::byte_buf buf_le = cista::serialize(s);
  cista::byte_buf buf_be =
      cista::serialize<cista::mode::SERIALIZE_BIG_ENDIAN>(s);

  EXPECT_TRUE(std::equal(buf_le.begin() + offsetof(u16string, s_.s_), buf_le.end(),
                         str_le.begin()));
  EXPECT_TRUE(std::equal(buf_be.begin() + offsetof(u16string, s_.s_), buf_be.end(),
                         str_be.begin()));

  auto const serialized_be =
      cista::deserialize<u16string, cista::mode::SERIALIZE_BIG_ENDIAN>(buf_be);

  EXPECT_EQ(*serialized_be, U16STR_SHORT_CORNER_CASE);
}

TEST(U16StringSerializationTest, EndianLong) {
  cista::byte_buf str_le = {0x30, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00,
                            0x34, 0x00, 0x35, 0x00, 0x36, 0x00, 0x37, 0x00};
  cista::byte_buf str_be = {0x00, 0x30, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33,
                            0x00, 0x34, 0x00, 0x35, 0x00, 0x36, 0x00, 0x37};
  u16string s = U16STR_LONG_CORNER_CASE;

  cista::byte_buf buf_le = cista::serialize(s);
  cista::byte_buf buf_be =
      cista::serialize<cista::mode::SERIALIZE_BIG_ENDIAN>(s);

  EXPECT_TRUE(std::equal(buf_le.begin() + sizeof(u16string), buf_le.end(),
                         str_le.begin()));
  EXPECT_TRUE(std::equal(buf_be.begin() + sizeof(u16string), buf_be.end(),
                         str_be.begin()));

  auto const serialized_be =
      cista::deserialize<u16string, cista::mode::SERIALIZE_BIG_ENDIAN>(buf_be);

  EXPECT_EQ(*serialized_be, U16STR_LONG_CORNER_CASE);
}

using cista::raw::u32string;
using cista::raw::u32string_view;

const auto constexpr U32STR_SHORT = U"01";
const auto constexpr U32STR_SHORT_CORNER_CASE = U"012";
const auto constexpr U32STR_LONG_CORNER_CASE = U"0123";
const auto constexpr U32STR_LONG = U"0123456789ABCDEF\U0001F4BB";

TEST(U32StringSerializationTest, Short) {
  u32string s = U32STR_SHORT;

  cista::byte_buf buf = cista::serialize(s);
  EXPECT_EQ(buf.size(), sizeof(u32string));

  auto const serialized = cista::deserialize<u32string>(buf);
  EXPECT_EQ(*serialized, U32STR_SHORT);
}

TEST(U32StringSerializationTest, Long) {
  u32string s = U32STR_LONG;

  cista::byte_buf buf = cista::serialize(s);
  EXPECT_EQ(buf.size(), sizeof(u32string) + s.size() * sizeof(char32_t));

  auto const serialized =
      cista::deserialize<u32string>(&buf[0], &buf[0] + buf.size());
  EXPECT_EQ(*serialized, U32STR_LONG);
}

TEST(U32StringSerializationTest, WithAdditionalChecks) {
  u32string s_s = U32STR_SHORT_CORNER_CASE, s_l = U32STR_LONG_CORNER_CASE;
  constexpr auto mode = cista::mode::WITH_VERSION |
                        cista::mode::WITH_INTEGRITY | cista::mode::DEEP_CHECK;

  cista::byte_buf buf_s = cista::serialize<mode>(s_s),
                  buf_l = cista::serialize<mode>(s_l);
  u32string *serialized_s, *serialized_l;

  EXPECT_NO_THROW((serialized_s = cista::deserialize<u32string, mode>(buf_s)));
  EXPECT_EQ(*serialized_s, U32STR_SHORT_CORNER_CASE);
  EXPECT_NO_THROW((serialized_l = cista::deserialize<u32string, mode>(buf_l)));
  EXPECT_EQ(*serialized_l, U32STR_LONG_CORNER_CASE);
}

TEST(U32StringSerializationTest, EndianShort) {
  cista::byte_buf str_le = {0x30, 0x00, 0x00, 0x00, 0x31, 0x00,
                            0x00, 0x00, 0x32, 0x00, 0x00, 0x00};
  cista::byte_buf str_be = {0x00, 0x00, 0x00, 0x30, 0x00, 0x00,
                            0x00, 0x31, 0x00, 0x00, 0x00, 0x32};
  u32string s = U32STR_SHORT_CORNER_CASE;

  cista::byte_buf buf_le = cista::serialize(s);
  cista::byte_buf buf_be =
      cista::serialize<cista::mode::SERIALIZE_BIG_ENDIAN>(s);

  EXPECT_TRUE(std::equal(buf_le.begin() + offsetof(u32string, s_.s_), buf_le.end(),
                         str_le.begin()));
  EXPECT_TRUE(std::equal(buf_be.begin() + offsetof(u32string, s_.s_), buf_be.end(),
                         str_be.begin()));

  auto const serialized_be =
      cista::deserialize<u32string, cista::mode::SERIALIZE_BIG_ENDIAN>(buf_be);

  EXPECT_EQ(*serialized_be, U32STR_SHORT_CORNER_CASE);
}

TEST(U32StringSerializationTest, EndianLong) {
  cista::byte_buf str_le = {0x30, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00,
                            0x32, 0x00, 0x00, 0x00, 0x33, 0x00, 0x00, 0x00};
  cista::byte_buf str_be = {0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x31,
                            0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x33};
  u32string s = U32STR_LONG_CORNER_CASE;

  cista::byte_buf buf_le = cista::serialize(s);
  cista::byte_buf buf_be =
      cista::serialize<cista::mode::SERIALIZE_BIG_ENDIAN>(s);

  EXPECT_TRUE(std::equal(buf_le.begin() + sizeof(u32string), buf_le.end(),
                         str_le.begin()));
  EXPECT_TRUE(std::equal(buf_be.begin() + sizeof(u32string), buf_be.end(),
                         str_be.begin()));

  auto const serialized_be =
      cista::deserialize<u32string, cista::mode::SERIALIZE_BIG_ENDIAN>(buf_be);

  EXPECT_EQ(*serialized_be, U32STR_LONG_CORNER_CASE);
}
