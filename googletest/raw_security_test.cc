#include <gtest/gtest.h>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/serialization.h"
#endif

namespace data = cista::raw;

namespace {

void TestSecValueOverflow() {
  struct serialize_me {
    int a_{0};
    struct inner {
      int b_{0};
      int c_{0};
      data::string d_;
    } j_;
  };

  cista::byte_buf buf;

  {
    serialize_me obj{1, {2, 3, data::string{"testtes"}}};
    buf = cista::serialize(obj);
  }

  EXPECT_EQ(buf.size(), sizeof(serialize_me));
  buf.resize(buf.size() - 1);

  ASSERT_THROW(cista::deserialize<serialize_me>(buf), std::exception);
}

void TestSecStringOverflow() {
  auto constexpr long_str = "The quick brown fox jumps over the lazy dog";

  struct serialize_me {
    int a_{0};
    struct inner {
      int b_{0};
      int c_{0};
      data::string d_;
    } j_;
  };

  cista::byte_buf buf;

  {
    serialize_me obj{1, {2, 3, data::string{long_str}}};
    buf = cista::serialize(obj);
  }

  EXPECT_EQ(buf.size(), sizeof(serialize_me) + std::strlen(long_str));
  buf.resize(buf.size() - 1);

  ASSERT_THROW(cista::deserialize<serialize_me>(buf), std::exception);
}

void TestSecVectorOverflow() {
  struct serialize_me {
    int a_{0};
    struct inner {
      int b_{0};
      int c_{0};
      data::vector<int> d_;
    } j_;
  };

  cista::byte_buf buf;

  {
    serialize_me obj{1, {2, 3, data::vector<int>{}}};
    obj.j_.d_.push_back(1);
    buf = cista::serialize(obj);
  }

  EXPECT_EQ(buf.size(), sizeof(serialize_me) + sizeof(int));
  buf.resize(buf.size() - 1);
  ASSERT_THROW(cista::deserialize<serialize_me>(buf), std::exception);

  {
    serialize_me obj{1, {2, 3, data::vector<int>{}}};
    buf = cista::serialize(obj);
  }
  buf.resize(buf.size() - 1);
  ASSERT_THROW(cista::deserialize<serialize_me>(buf), std::exception);
}

void TestSecUniquePtrOverflowUnset() {
  struct serialize_me {
    int a_{0};
    struct inner {
      int b_{0};
      int c_{0};
      data::unique_ptr<int> d_;
    } j_;
  };

  cista::byte_buf buf;

  {
    serialize_me obj{1, {2, 3, data::unique_ptr<int>{}}};
    buf = cista::serialize(obj);
  }

  buf.resize(buf.size() - 1);
  ASSERT_THROW(cista::deserialize<serialize_me>(buf), std::exception);
}

void TestSecUniquePtrOverflowSet() {
  struct serialize_me {
    int a_{0};
    struct inner {
      int b_{0};
      int c_{0};
      data::unique_ptr<int> d_;
    } j_;
  };

  cista::byte_buf buf;

  {
    serialize_me obj{1, {2, 3, data::unique_ptr<int>{}}};
    obj.j_.d_ = data::make_unique<int>(3);
    buf = cista::serialize(obj);
  }

  EXPECT_EQ(buf.size(), sizeof(serialize_me) + sizeof(int));
  buf.resize(buf.size() - 1);
  ASSERT_THROW(cista::deserialize<serialize_me>(buf), std::exception);
}

void TestSecArrayOverflow() {
  struct serialize_me {
    int a_{0};
    struct inner {
      int b_{0};
      int c_{0};
      data::array<int, 3> d_;
    } j_;
  };

  cista::byte_buf buf;

  {
    serialize_me obj{1, {2, 3, {}}};
    obj.j_.d_[0] = 1;
    obj.j_.d_[1] = 2;
    obj.j_.d_[2] = 3;
    buf = cista::serialize(obj);
  }

  EXPECT_EQ(buf.size(), sizeof(serialize_me));
  buf.resize(buf.size() - 1);
  ASSERT_THROW(cista::deserialize<serialize_me>(buf), std::exception);
}

}  // namespace

TEST(SecRawTest, ValueOverflow) { TestSecValueOverflow(); }
TEST(SecRawTest, StringOverflow) { TestSecStringOverflow(); }
TEST(SecRawTest, VectorOverflow) { TestSecVectorOverflow(); }
TEST(SecRawTest, UniquePtrOverflowUnset) { TestSecUniquePtrOverflowUnset(); }
TEST(SecRawTest, UniquePtrOverflowSet) { TestSecUniquePtrOverflowSet(); }
TEST(SecRawTest, ArrayOverflow) { TestSecArrayOverflow(); }