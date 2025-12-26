#include <memory>
#include <set>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>

#include <gtest/gtest.h>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/containers/vector.h"
#include "cista/equal_to.h"
#include "cista/is_iterable.h"
#include "cista/reflection/printable.h"
#include "cista/serialization.h"
#include "cista/type_hash/type_name.h"
#include "cista/verify.h"
#endif

// --- Helper for printing cista::raw::vector in GTest error messages ---
namespace cista::raw {
template <typename T>
std::ostream& operator<<(std::ostream& os, const vector<T>& v) {
  os << "{";
  for (size_t i = 0; i < v.size(); ++i) {
    os << v[i] << (i < v.size() - 1 ? ", " : "");
  }
  os << "}";
  return os;
}
} // namespace cista::raw

TEST(VectorInsertTest, InsertBegin) {
  using cista::raw::vector;

  auto v = vector<int>{1, 2, 3};
  v.insert(begin(v), 0);

  auto expected = vector<int>{0, 1, 2, 3};
  EXPECT_EQ(v, expected);
}

TEST(VectorInsertTest, InsertEnd) {
  using cista::raw::vector;

  auto v = vector<int>{1, 2, 3};
  v.insert(end(v), 4);

  auto expected = vector<int>{1, 2, 3, 4};
  EXPECT_EQ(v, expected);
}

TEST(VectorInsertTest, InsertMiddle) {
  using cista::raw::vector;

  auto v = vector<int>{1, 2, 3};
  v.insert(begin(v) + 1, 4);

  auto expected = vector<int>{1, 4, 2, 3};
  EXPECT_EQ(v, expected);
}

TEST(VectorInsertTest, RangeInsertBegin) {
  using cista::raw::vector;

  auto v = vector<int>{1, 2, 3};
  auto const w = vector<int>{8, 9};
  v.insert(begin(v), begin(w), end(w));

  auto expected = vector<int>{8, 9, 1, 2, 3};
  EXPECT_EQ(v, expected);
}

TEST(VectorInsertTest, RangeInsertEnd) {
  using cista::raw::vector;

  auto v = vector<int>{1, 2, 3};
  auto const w = vector<int>{8, 9};
  v.insert(end(v), begin(w), end(w));

  auto expected = vector<int>{1, 2, 3, 8, 9};
  EXPECT_EQ(v, expected);
}

TEST(VectorInsertTest, RangeInsertMiddle) {
  using cista::raw::vector;

  auto v = vector<int>{1, 2, 3};
  auto const w = vector<int>{8, 9};
  v.insert(begin(v) + 1, begin(w), end(w));

  auto expected = vector<int>{1, 8, 9, 2, 3};
  EXPECT_EQ(v, expected);
}

TEST(VectorInsertTest, RangeInsertNothing) {
  using cista::raw::vector;

  auto v = vector<int>{1, 2, 3};
  auto const w = vector<int>{};
  v.insert(begin(v) + 1, begin(w), end(w));

  auto expected = vector<int>{1, 2, 3};
  EXPECT_EQ(v, expected);
}

struct move_only_int {
  explicit move_only_int(int i) : i_{i} {}
  move_only_int(move_only_int const&) = delete;
  move_only_int(move_only_int&& o) = default;
  move_only_int& operator=(move_only_int const&) = delete;
  move_only_int& operator=(move_only_int&& o) = default;
  ~move_only_int() = default;

  friend bool operator==(move_only_int const& lhs, move_only_int const& rhs) {
    return lhs.i_ == rhs.i_;
  }

  friend std::ostream& operator<<(std::ostream& out, move_only_int const& e) {
    out << e.i_;
    return out;
  }

  int i_;
};

cista::raw::vector<move_only_int> make_move_only_int_vector(
    std::initializer_list<int> init) {
  cista::raw::vector<move_only_int> v;
  for (auto i : init) {
    v.emplace_back(i);
  }
  return v;
}

TEST(VectorInsertTest, RangeInsertMiddleMoveOnly) {
  auto v = make_move_only_int_vector({1, 2, 3});
  auto w = make_move_only_int_vector({8, 9});

  v.insert(begin(v) + 1, std::make_move_iterator(begin(w)),
           std::make_move_iterator(end(w)));

  auto expected = make_move_only_int_vector({1, 8, 9, 2, 3});
  EXPECT_EQ(v, expected);
}

struct input_iterator {
  using iterator_category = std::input_iterator_tag;
  using value_type = move_only_int;
  using difference_type = int;
  using pointer = move_only_int*;
  using reference = move_only_int&;

  explicit input_iterator(int i) : i_{i}, consumed_{std::make_shared<int>(i)} {}

  input_iterator& operator++() {
    cista::verify(i_ == *consumed_, "input_iterator: iterate a stale copy");
    *consumed_ = ++i_;
    return *this;
  }

  move_only_int operator*() const {
    cista::verify(i_ == *consumed_, "input_iterator: dereference a stale copy");
    return move_only_int{i_};
  }

  friend bool operator==(input_iterator const& lhs, input_iterator const& rhs) {
    return lhs.i_ == rhs.i_;
  }

  friend bool operator!=(input_iterator const& lhs, input_iterator const& rhs) {
    return !(lhs == rhs);
  }

  int i_;
  std::shared_ptr<int> consumed_;
};

TEST(VectorInputIteratorTest, InputIteratorWorks) {
  input_iterator a{42};
  auto cpy = a;
  ++a;
  EXPECT_THROW(++cpy, std::runtime_error);
  EXPECT_THROW(*cpy, std::runtime_error);
}

TEST(VectorInsertTest, RangeInsertInputIteratorBegin) {
  auto v = make_move_only_int_vector({1, 2, 3});
  v.insert(begin(v), input_iterator{8}, input_iterator{10});

  auto expected = make_move_only_int_vector({8, 9, 1, 2, 3});
  EXPECT_EQ(v, expected);
}

TEST(VectorInsertTest, RangeInsertInputIteratorEnd) {
  auto v = make_move_only_int_vector({1, 2, 3});
  v.insert(end(v), input_iterator{8}, input_iterator{10});

  auto expected = make_move_only_int_vector({1, 2, 3, 8, 9});
  EXPECT_EQ(v, expected);
}

TEST(VectorInsertTest, RangeInsertInputIteratorMiddle) {
  auto v = make_move_only_int_vector({1, 2, 3});
  v.insert(begin(v) + 1, input_iterator{8}, input_iterator{10});

  auto expected = make_move_only_int_vector({1, 8, 9, 2, 3});
  EXPECT_EQ(v, expected);
}

TEST(VectorInsertTest, RangeInsertInputIteratorNothing) {
  auto v = make_move_only_int_vector({1, 2, 3});
  v.insert(begin(v) + 1, input_iterator{0}, input_iterator{0});

  auto expected = make_move_only_int_vector({1, 2, 3});
  EXPECT_EQ(v, expected);
}

TEST(VectorEraseTest, EraseDuplicates) {
  using cista::raw::vector;

  auto v = vector<int>{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 9};
  std::sort(begin(v), end(v));
  v.erase(std::unique(begin(v), end(v)), end(v));

  auto expected = vector<int>{1, 2, 3, 4, 5, 6, 9};
  EXPECT_EQ(v, expected);
}

TEST(VectorComparisonTest, IterableComparison) {
  std::vector<int> std_v{1, 2, 3};
  cista::raw::vector<int> cista_v{1, 2, 3};
  std::set<int> std_s{1, 2, 3};
  EXPECT_TRUE(cista::equal_to<decltype(std_v)>{}(std_v, cista_v));
  EXPECT_TRUE(cista::equal_to<decltype(std_s)>{}(std_s, cista_v));
}

TEST(VectorConversionTest, ToVec) {
  namespace data = cista::raw;

  cista::buf<std::vector<uint8_t>> buf;
  for (auto i = 0U; i != 3; ++i) {  // Serialize.
    buf.reset();

    auto const v = std::vector<double>({1.0, 2.0});
    auto o = data::to_vec(v);
    EXPECT_EQ(o, data::to_vec(v, [](auto&& e) { return e; }));
    EXPECT_EQ(o, data::to_vec(begin(v), end(v), [](auto&& e) { return e; }));
    cista::serialize(buf, o);
  }

  // Deserialize.
  std::string s{reinterpret_cast<char*>(&buf.buf_[0]), buf.buf_.size()};
  auto const& deserialized =
      *cista::deserialize<data::vector<double>>(&s[0], &s[0] + s.size());
  ASSERT_EQ(deserialized.size(), 2);
  EXPECT_DOUBLE_EQ(deserialized[0], 1.0);
  EXPECT_DOUBLE_EQ(deserialized[1], 2.0);
}

TEST(VectorConversionTest, ToIndexedVec) {
  namespace data = cista::raw;

  std::vector<unsigned char> buf;
  {  // Serialize.
    auto const v = std::vector<double>({1.0, 2.0});
    auto o = data::to_indexed_vec(v);
    EXPECT_EQ(o, data::to_indexed_vec(v, [](auto&& e) { return e; }));
    EXPECT_EQ(o,
              data::to_indexed_vec(begin(v), end(v), [](auto&& e) { return e; }));
    buf = cista::serialize(o);
  }

  // Deserialize.
  auto const& deserialized =
      *cista::deserialize<data::indexed_vector<double>>(buf);
  ASSERT_EQ(deserialized.size(), 2);
  EXPECT_DOUBLE_EQ(deserialized[0], 1.0);
  EXPECT_DOUBLE_EQ(deserialized[1], 2.0);
}

TEST(VectorInsertTest, ComplexTypeInsert) {
  namespace data = cista::raw;

  auto v = data::vector<data::vector<data::string>>{
      {"abc", "def", "longlonglonglonglonglonglonglonglonglonglongstring"},
      {"uvw", "xyz", "longlonglonglonglonglonglonglonglonglonglongstring"}};
  auto u = v;
  for (int i = 0; i < 10; ++i) {
    auto const v_copy = v;
    v.insert(begin(v), begin(u), end(u));
    u.insert(begin(u), begin(v_copy), end(v_copy));
  }
  ASSERT_EQ(u.size(), v.size());
  ASSERT_EQ(u.size(), 2048);
}

TEST(VectorEraseTest, EraseThenSerialize) {
  using cista::raw::vector;

  std::vector<uint8_t> buf;
  {
    auto v = vector<int>{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 9};
    v.erase(std::remove_if(begin(v), end(v), [](auto&& i) { return i > 5; }),
            end(v));

    buf = cista::serialize(v);
  }

  auto deserialized = cista::deserialize<vector<int>>(buf);
  ASSERT_NE(deserialized, nullptr);
  auto expected = vector<int>{3, 1, 4, 1, 5, 2, 5, 3, 5};
  EXPECT_EQ(*deserialized, expected);
}

TEST(VectorEraseTest, EraseUntilEmptyThenSerialize) {
  using cista::raw::vector;

  std::vector<uint8_t> buf;

  {
    auto v = vector<int>{3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 9};
    v.erase(std::remove_if(begin(v), end(v), [](auto&& i) { return i != 10; }),
            end(v));

    buf = cista::serialize(v);
  }

  auto deserialized = cista::deserialize<vector<int>>(buf);
  ASSERT_NE(deserialized, nullptr);
  auto expected = vector<int>{};
  EXPECT_EQ(*deserialized, expected);
}

TEST(VectorEraseTest, EraseLoop) {
  using cista::raw::vector;

  vector<int> uut{1, 2, 3};
  std::vector<int> ref{1, 2, 3};

  auto const verify_equality = [&]() {
    ASSERT_EQ(ref.size(), uut.size());
    for (auto i = 0U; i != ref.size(); ++i) {
      EXPECT_EQ(ref[i], uut[i]);
    }
  };

  auto uut_it = uut.begin();
  auto ref_it = ref.begin();
  while (!ref.empty()) {
    uut_it = uut.erase(uut_it);
    ref_it = ref.erase(ref_it);
    if (ref_it != end(ref)) {
      ASSERT_NE(uut_it, end(uut));
      EXPECT_EQ(*ref_it, *uut_it);
    }
    verify_equality();
  }
}