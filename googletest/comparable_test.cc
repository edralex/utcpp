#include <string>

#include "gtest/gtest.h"

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/reflection/comparable.h"
#endif

struct comparable_a {
  CISTA_FRIEND_COMPARABLE(comparable_a)
  int i_ = 1;
  int j_ = 2;
  double d_ = 100.0;
  std::string s_ = "hello world";
};

TEST(ComparableTest, EqualityAndComparison) {
  comparable_a inst1, inst2;

  EXPECT_EQ(inst1, inst2);
  EXPECT_FALSE(inst1 != inst2);
  EXPECT_LE(inst1, inst2);
  EXPECT_GE(inst1, inst2);
  EXPECT_FALSE(inst1 < inst2);
  EXPECT_FALSE(inst1 > inst2);

  inst1.j_ = 1;

  EXPECT_NE(inst1, inst2);
  EXPECT_FALSE(inst1 == inst2);
  EXPECT_LE(inst1, inst2);
  EXPECT_FALSE(inst1 >= inst2);
  EXPECT_LT(inst1, inst2);
  EXPECT_FALSE(inst1 > inst2);
}
