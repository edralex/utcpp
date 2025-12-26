#include "gtest/gtest.h"

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/serialization.h"
#endif

namespace data = cista::offset;

TEST(OffsetSerializeTest, VectorSerialize) {
  cista::byte_buf buf;
  {
    data::vector<int32_t> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);

    int32_t j = 1;
    for (auto const i : vec) {
      EXPECT_EQ(i, j++);
    }

    buf = serialize(vec);
  }

  auto const vec = cista::deserialize<data::vector<int32_t>>(buf);
  int32_t j = 1;
  for (auto const i : *vec) {
    EXPECT_EQ(i, j++);
  }
}

TEST(OffsetSerializeTest, StringSerialize) {
  constexpr auto const s = "The quick brown fox jumps over the lazy dog";

  cista::byte_buf buf;
  {
    data::string str{s};
    buf = serialize(str);
  }

  auto const str = cista::deserialize<data::string>(buf);
  EXPECT_EQ(*str, s);
}

TEST(OffsetSerializeTest, UniquePtrSerialize) {
  cista::byte_buf buf;
  {
    auto const ptr = data::make_unique<int>(33);
    buf = serialize(ptr);
  }

  auto const ptr = cista::deserialize<data::unique_ptr<int>>(buf);
  EXPECT_EQ(**ptr, 33);
}

TEST(OffsetSerializeTest, OffsetPtrSerialize) {
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
  EXPECT_EQ(deserialized->raw_.get(), deserialized->i_.get());
  EXPECT_EQ(*deserialized->raw_, 77);
  EXPECT_EQ(*deserialized->i_.get(), 77);
}

TEST(OffsetSerializeTest, OffsetPtrSerializePending) {
  struct serialize_me {
    data::ptr<int> raw_{nullptr};
    data::unique_ptr<int> i_{data::make_unique<int>(77)};
  };

  cista::byte_buf buf;

  {
    serialize_me obj;
    obj.raw_ = obj.i_.get();
    buf = cista::serialize(obj);
  }  // EOL obj

  auto const serialized = cista::deserialize<serialize_me>(buf);
  EXPECT_EQ(serialized->raw_.get(), serialized->i_.get());
  EXPECT_EQ(*serialized->raw_, 77);
  EXPECT_EQ(*serialized->i_.get(), 77);
}
