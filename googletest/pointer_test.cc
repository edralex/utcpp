#include "gtest/gtest.h"

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/serialization.h"
#endif

namespace data = cista::raw;

TEST(PointerSerializationTest, PointerSerialization) {
  struct serialize_me {
    data::unique_ptr<int> i_{data::make_unique<int>(77)};
    data::ptr<int> raw_{i_.get()};
  };

  cista::byte_buf buf;

  {
    serialize_me obj;
    buf = cista::serialize(obj);
  }  // EOL obj

  auto const deserialized = cista::deserialize<serialize_me>(buf);
  EXPECT_EQ(deserialized->raw_, deserialized->i_.get());
  EXPECT_EQ(*deserialized->raw_, 77);
  EXPECT_EQ(*deserialized->i_.get(), 77);
}
