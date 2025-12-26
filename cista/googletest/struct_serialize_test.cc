#include <gtest/gtest.h>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/serialization.h"
#endif

TEST(StructSerializationTest, BasicStruct) {
  struct serialize_me {
    int a_{0};
    struct inner {
      int b_{0};
      int c_{0};
      cista::raw::string d_;
    } j_;
  };

  cista::byte_buf buf;

  {
    serialize_me obj{1, {2, 3, cista::raw::string{"testtes"}}};
    buf = cista::serialize(obj);
  }

  EXPECT_EQ(buf.size(), sizeof(serialize_me));

  auto const serialized =
      cista::deserialize<serialize_me>(&buf[0], &buf[0] + buf.size());
  ASSERT_NE(serialized, nullptr);
  EXPECT_EQ(serialized->a_, 1);
  EXPECT_EQ(serialized->j_.b_, 2);
  EXPECT_EQ(serialized->j_.c_, 3);
  EXPECT_EQ(serialized->j_.d_, cista::raw::string{"testtes"});
}