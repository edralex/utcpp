#include <gtest/gtest.h>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/reflection/for_each_field.h"
#endif

struct single_member_struct {
  int i_{7};
};

TEST(SingleMemberStructTest, ForEachField) {
  single_member_struct s;
  cista::for_each_field(s, [](auto&& f) {
    EXPECT_EQ(f, 7);
  });
}