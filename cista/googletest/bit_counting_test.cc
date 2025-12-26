#include <gtest/gtest.h>

#include <limits>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/bit_counting.h"
#endif

TEST(BitCounting64, Zero) {
  uint64_t t = 0;
  EXPECT_EQ(cista::trailing_zeros(t), 64);
  EXPECT_EQ(cista::leading_zeros(t), 64);
}

TEST(BitCounting64, One) {
  uint64_t t = 0'1;
  EXPECT_EQ(cista::trailing_zeros(t), 0);
  EXPECT_EQ(cista::leading_zeros(t), 63);
}

TEST(BitCounting64, Higher) {
  uint64_t t = std::numeric_limits<uint32_t>::max();
  t += 1;
  EXPECT_EQ(cista::trailing_zeros(t), 32);
  EXPECT_EQ(cista::leading_zeros(t), 31);
}

TEST(BitCounting64, Any) {
  uint64_t t = 7ULL << 30ULL;
  EXPECT_EQ(cista::trailing_zeros(t), 30);
  EXPECT_EQ(cista::leading_zeros(t), 31);
}

TEST(BitCounting32, Zero) {
  uint32_t t = 0;
  EXPECT_EQ(cista::trailing_zeros(t), 32);
  EXPECT_EQ(cista::leading_zeros(t), 32);
}

TEST(BitCounting32, One) {
  uint32_t t = 0'1;
  EXPECT_EQ(cista::trailing_zeros(t), 0);
  EXPECT_EQ(cista::leading_zeros(t), 31);
}

TEST(BitCounting32, Highest) {
  uint32_t t = 1U << 31U;
  EXPECT_EQ(cista::trailing_zeros(t), 31);
  EXPECT_EQ(cista::leading_zeros(t), 0);
}

TEST(BitCounting32, Any) {
  uint32_t t = 7U << 30U;
  EXPECT_EQ(cista::trailing_zeros(t), 30);
  EXPECT_EQ(cista::leading_zeros(t), 0);
}
