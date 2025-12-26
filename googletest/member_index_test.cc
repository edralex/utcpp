#include "gtest/gtest.h"

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/reflection/member_index.h"
#endif

TEST(MemberIndexTest, MemberIndex) {
  struct test {
    int a, b, c;
  };
  EXPECT_EQ(0, cista::member_index(&test::a));
  EXPECT_EQ(1, cista::member_index(&test::b));
  EXPECT_EQ(2, cista::member_index(&test::c));
}
