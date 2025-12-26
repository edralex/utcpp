#include <gtest/gtest.h>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/serialization.h"
#endif

namespace data = cista::offset;
using namespace cista;

static unsigned ctor{0}, cpy_ctor{0}, mov_ctor{0}, cpy_assign{0}, mov_assign{0},
    dtor{0};

void reset() {
  ctor = 0;
  cpy_ctor = 0;
  mov_ctor = 0;
  cpy_assign = 0;
  mov_assign = 0;
  dtor = 0;
}

struct array_test_struct {
  array_test_struct(int i) : i_{i} { ++ctor; }
  array_test_struct() { ++ctor; }
  array_test_struct(array_test_struct const& o) : i_{o.i_} { ++cpy_ctor; }
  array_test_struct(array_test_struct&& o) : i_{o.i_} { ++mov_ctor; }
  array_test_struct& operator=(array_test_struct const& o) {
    i_ = o.i_;
    ++cpy_assign;
    return *this;
  }
  array_test_struct& operator=(array_test_struct&& o) {
    i_ = o.i_;
    ++mov_assign;
    return *this;
  }
  ~array_test_struct() { ++dtor; }
  int i_{0};
};

TEST(ArrayTest, MoveReverse) {
  reset();

  {
    data::array<array_test_struct, 2> arr1, arr2;
    arr1[0].i_ = 1;
    arr1[1].i_ = 2;

    std::copy(std::make_move_iterator(arr1.rbegin()),
              std::make_move_iterator(arr1.rend()), std::begin(arr2));

    EXPECT_EQ(arr2.at(0).i_, 2);
    EXPECT_EQ(arr2[1].i_, 1);
  }

  EXPECT_EQ(ctor, 4);
  EXPECT_EQ(cpy_ctor, 0);
  EXPECT_EQ(mov_ctor, 0);
  EXPECT_EQ(cpy_assign, 0);
  EXPECT_EQ(mov_assign, 2);
  EXPECT_EQ(dtor, 4);
}

TEST(ArrayTest, CopyForward) {
  reset();

  {
    data::array<array_test_struct, 2> arr1, arr2;
    arr1[0].i_ = 1;
    arr1[1].i_ = 2;

    std::copy(std::begin(arr1), std::end(arr1), std::begin(arr2));

    EXPECT_EQ(arr2.at(0).i_, 1);
    EXPECT_EQ(arr2[1].i_, 2);
  }

  EXPECT_EQ(ctor, 4);
  EXPECT_EQ(cpy_ctor, 0);
  EXPECT_EQ(mov_ctor, 0);
  EXPECT_EQ(cpy_assign, 2);
  EXPECT_EQ(mov_assign, 0);
  EXPECT_EQ(dtor, 4);
}

TEST(ArrayTest, Serialize) {
  struct serialize_me {
    int a_{0};
    struct inner {
      int b_{0};
      int c_{0};
      data::array<int, 128> d_;
    } j_;
  };

  cista::byte_buf buf;

  {
    serialize_me obj{1, {2, 3, data::array<int, 128>{}}};
    obj.j_.d_[0] = 1;
    obj.j_.d_[1] = 2;
    obj.j_.d_[2] = 3;
    obj.j_.d_[3] = 4;
    buf = cista::serialize<mode::WITH_VERSION>(obj);
  }  // EOL obj

  auto const deserialize =
      cista::deserialize<serialize_me, mode::WITH_VERSION>(buf);
  EXPECT_EQ(deserialize->j_.d_[0], 1);
  EXPECT_EQ(deserialize->j_.d_[1], 2);
  EXPECT_EQ(deserialize->j_.d_[2], 3);
  EXPECT_EQ(deserialize->j_.d_[3], 4);
}
