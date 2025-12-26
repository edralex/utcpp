#include "gtest/gtest.h"

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/containers/offset_ptr.h"
#endif

TEST(OffsetPtrTest, OffsetPtrToVoid) {
  auto const a = 77;
  auto const ptr = cista::offset_ptr<void>{&a};
  auto const null = cista::offset_ptr<void>{nullptr};
  EXPECT_EQ(*reinterpret_cast<int const*>(ptr.get()), 77);
  EXPECT_FALSE(static_cast<bool>(null));
  EXPECT_TRUE(static_cast<bool>(ptr));
}
