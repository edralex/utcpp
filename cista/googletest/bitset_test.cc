#include <gtest/gtest.h>

#include <bitset>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/containers/bitset.h"
#include "cista/serialization.h"
#endif

TEST(Bitset, BasicOperations) {
  constexpr auto const size = 136;

  auto s = std::string{};
  for (int i = 0U; i != size; ++i) {
    s.push_back(i % 3 == 0U ? '1' : '0');
  }

  auto std_bits = std::bitset<size>{&s[0], s.size()};
  auto cista_bits = cista::bitset<size>{std::string_view{s}};

  EXPECT_EQ(std_bits.size(), cista_bits.size());

  for (auto i = 0U; i != size; ++i) {
    EXPECT_EQ(std_bits.test(size - i - 1U), (i % 3U == 0U));
    EXPECT_EQ(cista_bits.test(size - i - 1U), (i % 3U == 0U));
  }
  EXPECT_EQ(std_bits.to_string(), cista_bits.to_string());
  auto const mod_3_std = std_bits;
  auto const mod_3_cista = cista_bits;

  for (auto i = 0U; i != size; ++i) {
    cista_bits.set(i, i % 5 == 0);
    std_bits.set(i, i % 5 == 0);
  }

  for (auto i = 0U; i != size; ++i) {
    EXPECT_EQ(std_bits.test(i), (i % 5 == 0U));
    EXPECT_EQ(cista_bits.test(i), (i % 5 == 0U));
    EXPECT_EQ(std_bits[i], (i % 5 == 0U));
    EXPECT_EQ(cista_bits[i], (i % 5 == 0U));
  }
  EXPECT_EQ(std_bits.to_string(), cista_bits.to_string());
  auto const mod_5_std = std_bits;
  auto const mod_5_cista = cista_bits;

  EXPECT_EQ((~mod_3_std).to_string(), (~mod_3_cista).to_string());
  EXPECT_EQ((~mod_3_std).count(), (~mod_3_cista).count());

  auto mod_15_cista = mod_5_cista;
  mod_15_cista &= mod_3_cista;
  auto mod_15_std = mod_5_std;
  mod_15_std &= mod_3_std;
  EXPECT_EQ(mod_15_cista.to_string(), mod_15_std.to_string());
  EXPECT_EQ(mod_15_cista.count(), mod_15_std.count());

  auto mod_3_or_5_cista = mod_5_cista;
  mod_3_or_5_cista |= mod_3_cista;
  auto mod_3_or_5_std = mod_5_std;
  mod_3_or_5_std |= mod_3_std;
  EXPECT_EQ(mod_3_or_5_cista.to_string(), mod_3_or_5_std.to_string());
  EXPECT_EQ(mod_3_or_5_cista.count(), mod_3_or_5_std.count());

  auto x = mod_3_or_5_cista;
  mod_3_or_5_cista ^= mod_15_cista;
  auto y = mod_3_or_5_std;
  mod_3_or_5_std ^= mod_15_std;
  EXPECT_EQ(x.to_string(), y.to_string());

  mod_3_or_5_cista >>= 67U;
  mod_3_or_5_std >>= 67U;
  EXPECT_EQ(mod_3_or_5_cista.to_string(), mod_3_or_5_std.to_string());
  EXPECT_EQ(mod_3_or_5_cista.count(), mod_3_or_5_std.count());

  mod_3_or_5_cista <<= 67U;
  mod_3_or_5_std <<= 67U;
  EXPECT_EQ(mod_3_or_5_cista.to_string(), mod_3_or_5_std.to_string());
  EXPECT_EQ(mod_3_or_5_cista.count(), mod_3_or_5_std.count());
}

TEST(Bitset, ShiftRight) {
  auto const z =
      "000000000000000000000000000000000000000000000000000000000000000000000000"
      "00000";
  auto a = cista::bitset<77>{std::string_view{z}};
  auto b = std::bitset<77>{std::string{z}};
  a >>= 48;
  b >>= 48;
  EXPECT_EQ(a.to_string(), b.to_string());
  EXPECT_EQ(a.count(), b.count());
}

TEST(Bitset, Serialize) {
  constexpr auto const mode =
      cista::mode::WITH_INTEGRITY | cista::mode::WITH_VERSION;

  constexpr auto const size = 512;
  using bitfield = cista::bitset<size>;

  cista::byte_buf buf;

  auto s = std::string{};
  for (int i = 0U; i != size; ++i) {
    s.push_back(i % 5 == 0U ? '1' : '0');
  }

  {
    auto obj = cista::bitset<size>{std::string_view{s}};
    buf = cista::serialize<mode>(obj);
  }  // EOL obj

  auto const& deserialized = *cista::unchecked_deserialize<bitfield, mode>(buf);
  EXPECT_EQ(deserialized, bitfield{std::string_view(s)});
}
