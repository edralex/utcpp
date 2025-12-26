#include "gtest/gtest.h"

#include <cinttypes>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/serialization.h"
#endif

using namespace cista;
using namespace cista::raw;

struct serialize_me {
  uint32_t v1_ : 10;
  uint32_t v2_ : 10;
  uint32_t v3_ : 10;
  uint32_t v4_ : 1;
  uint32_t v5_ : 1;
};

template <typename Ctx>
void serialize(Ctx&, serialize_me const*, offset_t const) {}

template <typename Ctx>
void deserialize(Ctx const&, serialize_me*) {}

TEST(CustomStructTest, CustomStruct) {
  byte_buf buf;

  {
    serialize_me obj{1, 2, 3, 0, 1};
    buf = serialize(obj);
  }  // EOL obj

  auto const serialized = unchecked_deserialize<serialize_me>(buf);
  EXPECT_EQ(1, serialized->v1_);
  EXPECT_EQ(2, serialized->v2_);
  EXPECT_EQ(3, serialized->v3_);
  EXPECT_EQ(0, serialized->v4_);
  EXPECT_EQ(1, serialized->v5_);
}
